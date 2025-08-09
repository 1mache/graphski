#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <limits>
#include <algorithm>

#include "json.hpp"
#include "IGraph.h"

namespace graphski
{
    template<typename NodeT, typename EdgeT>
    class Graph : public IGraph
    {
	protected: // because only classes that see m_adjList need this
        struct EdgeLocator
        {
            EdgeLocator(NodeId _nodeId, size_t _neighborId) :
                nodeId(_nodeId), neighborId(_neighborId) {}
            NodeId nodeId;
            size_t neighborId; // index of the neighbor in the node's edges vector
        };

    public:
        Graph(size_t reserveCount = 0);
        
        Graph(Graph&) = delete;
        Graph& operator=(Graph&) = delete;

        Graph(Graph&& other); 
        Graph& operator=(Graph&& other); 

        virtual ~Graph(); 

        // clears the graph
        virtual void makeEmpty() override; 
        // how many nodes are there
        NodeId nodeCount() const override; 
        // how many edges are there
        size_t edgeCount() const override; 
        // peek node info by id
        NodePeek peekNode(NodeId id) const override; 
        // creates a node with empty edges list, returns its unique id
        virtual NodeId addNode(std::string name = "") override; 
        // creates an edge between to given nodes, gets them by ids
        virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) override; 
		// deletes an edge between two nodes, gets them by ids
		virtual void deleteEdge(NodeId fromNodeId, NodeId toNodeId);
        // marks the node of given id
        virtual void markNode(NodeId id, bool val = true) override; 
        // gets an array of neighbor ids for the given node id
        std::vector<NodeId> getNeighbors(NodeId id) const override; 
        // gets a peek of the adjacency list. all in terms of ids, not pointers
        AdjacencyListPeek getAdjacencyList() const override; 
        // transposes the graph 
        virtual void transpose(); 
        // saves the graph to a file in json format
        void saveToFile() const; 
        // loads the graph from a file in json format
        virtual void loadFromFile(); 

    protected:
        bool nodeIdInBounds(NodeId id) const; 
        NodeT* getNode(NodeId id);
        const NodeT* getNode(NodeId id) const; // const version
        EdgeT* getEdge(const EdgeLocator& edgeId);
        const EdgeT* getEdge(const EdgeLocator& edgeId) const; // const version

        // returns the json representation of the node, used in saveToFile
        virtual nlohmann::json serializeNode(const NodeT* node) const; 
        // creates and returns a new node given json representation of it, used in loadFromFile
        virtual NodeT* deserializeNode(const nlohmann::json& j) const; 

        // adjacency list contains pairs node : its edges
        using AdjacencyList = std::vector<std::pair<NodeT*, std::vector<EdgeT*>>>;
        AdjacencyList m_adjList;

    private:
        // deletes all nodes and edges
        void deleteAdjList(); 

        static constexpr NodeId MAX_NODES = std::numeric_limits<NodeId>::max();
        static constexpr NodeId INIT_NODES = 10;
		
        // TODO: make this dynamic
        static constexpr const char* FILE_NAME = "graph.json";
    };

    // ================= Implementation =================

    template<typename NodeT, typename EdgeT>
    Graph<NodeT, EdgeT>::Graph(size_t reserveCount)
    {
        if (reserveCount > size_t(MAX_NODES))
        {
            std::cout << "Too much nodes requested, max is " << MAX_NODES << '\n';
            reserveCount = size_t(INIT_NODES);
        }
        else if (reserveCount == 0)
            reserveCount = size_t(INIT_NODES);

        m_adjList.reserve(reserveCount);
    }

    template<typename NodeT, typename EdgeT>
    Graph<NodeT, EdgeT>::Graph(Graph&& other)
    {
        deleteAdjList();
        m_adjList = std::move(other.m_adjList);
    }

    template<typename NodeT, typename EdgeT>
    Graph<NodeT, EdgeT>& Graph<NodeT, EdgeT>::operator=(Graph&& other)
    {
        if (this != &other)
        {
            deleteAdjList();
            m_adjList = std::move(other.m_adjList);
        }
        return *this;
    }

    template<typename NodeT, typename EdgeT>
    Graph<NodeT, EdgeT>::~Graph()
    {
        deleteAdjList();
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::makeEmpty()
    {
        deleteAdjList();
        m_adjList.clear();
        m_adjList.reserve(INIT_NODES);
    }

    template<typename NodeT, typename EdgeT>
    NodeId Graph<NodeT, EdgeT>::nodeCount() const
    {
        return (NodeId)m_adjList.size();
    }

    template<typename NodeT, typename EdgeT>
    size_t Graph<NodeT, EdgeT>::edgeCount() const
    {
        size_t count = 0;
        for (const auto& pair : m_adjList)
        {
            count += pair.second.size();
        }
        return count;
    }

    template<typename NodeT, typename EdgeT>
    NodePeek Graph<NodeT, EdgeT>::peekNode(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return { 0, "", 0, 0 };

        const NodeT* node = m_adjList[id].first;
        return NodePeek(*node);
    }

    template<typename NodeT, typename EdgeT>
    NodeId Graph<NodeT, EdgeT>::addNode(std::string name)
    {
        NodeId id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)

        if (id >= MAX_NODES)
			throw std::overflow_error("Maximum number of nodes exceeded");

        m_adjList.push_back({ new NodeT(id, name), {} });
        return id;
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::addEdge(NodeId fromNodeId, NodeId toNodeId)
    {
        NodeT* fromPtr = getNode(fromNodeId),
            * toPtr = getNode(toNodeId);

        if (!fromPtr || !toPtr)
        {
            std::cerr << "Error: trying to add edge between non-existing nodes: "
                << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
			throw std::invalid_argument("Invalid node id(s) provided for edge creation.");
        }

        // check if edge already exists
        for (const EdgeT* edge : m_adjList[fromNodeId].second)
        {
            if (edge->getTo()->getId() == toNodeId)
				return; // edge already exists, do not add it again
        }

        EdgeT* newEdge = new EdgeT(fromPtr, toPtr);

        // increment degrees
        fromPtr->setDOut(fromPtr->getDOut() + 1);
        toPtr->setDIn(toPtr->getDIn() + 1);

        m_adjList[fromNodeId].second.push_back(newEdge);
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::deleteEdge(NodeId fromNodeId, NodeId toNodeId)
    {
        NodeT* fromPtr = getNode(fromNodeId),
            * toPtr = getNode(toNodeId);

        if (!fromPtr || !toPtr)
        {
            std::cerr << "Error: trying to add edge between non-existing nodes: "
                << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
            throw std::invalid_argument("Invalid node id(s) provided for edge creation.");
        }

		auto& edges = m_adjList[fromNodeId].second;
        auto it = std::find_if(edges.begin(),edges.end(),
            [&](EdgeT* edge) { return edge->getTo()->getId() == toNodeId; });

        if(it == edges.end())
            return; // edge does not exist, nothing to delete

		edges.erase(it);
        fromPtr->setDOut(fromPtr->getDOut() - 1);
		toPtr->setDIn(toPtr->getDIn() - 1);
	
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::markNode(NodeId id, bool val)
    {
        auto* node = getNode(id);
		if (node)
            node->mark(val);
    }

    template<typename NodeT, typename EdgeT>
    std::vector<NodeId> Graph<NodeT, EdgeT>::getNeighbors(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return {};

        std::vector<NodeId> result;

		auto* node = getNode(id);
        if(!node)
			throw std::invalid_argument("Node with given id does not exist.");

        result.reserve(node->getDOut());

        for (const EdgeT* edge : m_adjList[id].second)
        {
            result.push_back(edge->getTo()->getId());
        }

        return result;
    }

    template<typename NodeT, typename EdgeT>
    AdjacencyListPeek Graph<NodeT, EdgeT>::getAdjacencyList() const
    {
        AdjacencyListPeek result;
        result.reserve(m_adjList.size());
        for (const auto& pair : m_adjList)
        {
            NodeT* node = pair.first;
            result.push_back({ NodePeek(*node), getNeighbors(node->getId()) });
        }
        return result;
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::transpose()
    {
        std::cout << "I am transposing it" << std::endl;

        // new Adjacency list sized like the original and filled with empty pairs
        AdjacencyList newAdjList{ m_adjList.size(), {nullptr, {}} };

        // fill it with nodes
        for (const auto& pair : m_adjList)
        {
            NodeT* node = pair.first;

            NodeT* newNode = new NodeT(*node);
            // swap in and out degrees for new node
            newNode->setDIn(node->getDOut());
            newNode->setDOut(node->getDIn());

            newAdjList[newNode->getId()] = { newNode, {} };
        }

        // fill it with opposite edges
        for (const auto& pair : m_adjList)
        {
            // pointer to new node with same id from new list
            NodeId nodeId = pair.first->getId();
            auto& edges = pair.second;

            for (const auto& edge : edges)
            {
                NodeId targetId = edge->getTo()->getId();
                // its crucial to use newAdjList here, not m_adjList when accessing nodes
                auto* edgeTarget = newAdjList[targetId].first;
                newAdjList[targetId].second.push_back(
                    new EdgeT(edgeTarget, newAdjList[nodeId].first)
                );
            }
        }

        // free previous adjlist
        deleteAdjList();
        m_adjList = std::move(newAdjList);
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::saveToFile() const
    {
        // initialize the file
        nlohmann::json j;
        // write the number of nodes
        j["nodeCount"] = nodeCount();
        // initialize the array of neighbors in json 
        auto& nodesArr = j["nodes"] = nlohmann::json::array();

        for (const auto& pair : m_adjList)
        {
            auto nodeJson = serializeNode(pair.first);

            std::vector<NodeId> neighbors;
            for (const EdgeT* edge : pair.second)
                // get all the id's of the nodes that are connected to this one
                neighbors.push_back(static_cast<const NodeT*>(edge->getTo())->getId());

            nodeJson["neighbors"] = neighbors;
            nodesArr.push_back(nodeJson);
        }

        std::ofstream file(FILE_NAME);
        if (!file.is_open())
        {
            std::cout << "Error opening file for writing: " << FILE_NAME << std::endl;
        }

        file << j.dump(2); // pretty print with 2 spaces (you can change this)
        file.close();
        std::cout << "Graph saved to " << FILE_NAME << std::endl;
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::loadFromFile()
    {
        nlohmann::json j;
        std::ifstream file(FILE_NAME);
        if (file.is_open())
        {
            file >> j;
            file.close();
        }
        else
        {
            std::cout << "Error opening file" << std::endl;
            return;
        }

        makeEmpty(); // clear the graph before loading
        m_adjList.reserve(j["nodeCount"].get<NodeId>());

        // create nodes
        for (auto& node : j["nodes"])
            m_adjList.push_back({ deserializeNode(node), {} });

        // add edges
        for (auto& node : j["nodes"])
        {
            NodeId id = node["id"];
            for (NodeId neighbor : node["neighbors"])
                addEdge(id, neighbor);
        }
    }

    template<typename NodeT, typename EdgeT>
    bool Graph<NodeT, EdgeT>::nodeIdInBounds(NodeId id) const
    {
        if (id >= nodeCount())
        {
            std::cerr << "Node id out of bounds: " << (int)id << std::endl;
            return false;
        }
        return true;
    }

    template<typename NodeT, typename EdgeT>
    NodeT* Graph<NodeT, EdgeT>::getNode(NodeId id)
    {
        if (!nodeIdInBounds(id))
            return nullptr;
        return m_adjList[id].first;
    }

    template<typename NodeT, typename EdgeT>
    const NodeT* Graph<NodeT, EdgeT>::getNode(NodeId id) const
    {
        return const_cast<Graph*>(this)->getNode(id);
    }

    template<typename NodeT, typename EdgeT>
    EdgeT* Graph<NodeT, EdgeT>::getEdge(const EdgeLocator& edgeId)
    {
        if (!nodeIdInBounds(edgeId.nodeId))
            return nullptr;
        
		auto& neighbors = m_adjList[edgeId.nodeId].second;
		return neighbors[edgeId.neighborId]; // return the edge at the index
    }

    template<typename NodeT, typename EdgeT>
    const EdgeT* Graph<NodeT, EdgeT>::getEdge(const EdgeLocator& edgeId) const
    {
		return const_cast<Graph*>(this)->getEdge(edgeId);
    }

    template<typename NodeT, typename EdgeT>
    nlohmann::json Graph<NodeT, EdgeT>::serializeNode(const NodeT* node) const
    {
        nlohmann::json j;
        j["id"] = node->getId();
        j["name"] = node->getName();
        return j;
    }

    template<typename NodeT, typename EdgeT>
    NodeT* Graph<NodeT, EdgeT>::deserializeNode(const nlohmann::json& j) const
    {
        return new NodeT(j["id"].get<NodeId>(),
            j["name"].get<std::string>());
    }

    template<typename NodeT, typename EdgeT>
    void Graph<NodeT, EdgeT>::deleteAdjList()
    {
        for (auto pair : m_adjList)
        {
            auto* node = pair.first;

            // delete all edges of the node
            for (auto* edge : pair.second)
            {
                delete edge;
            }
            // delete the node
            delete node;
        }
    }
}