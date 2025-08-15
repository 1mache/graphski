#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <limits>
#include <algorithm>

#include "json.hpp"
#include "IGraph.h"
#include "Node.h"
#include "Edge.h"

namespace graphski
{
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

        virtual ~Graph()
        {
            deleteAdjList();
        }

        // clears the graph
        virtual void makeEmpty() override;
        // how many nodes are there
        NodeId nodeCount() const override
        {
            return (NodeId)m_adjList.size();
        }
        // how many edges are there
        size_t edgeCount() const override; 
        // peek node info by id
        NodePeek peekNode(NodeId id) const override; 
        // creates a node with empty edges list, returns its unique id
        virtual NodeId addNode(std::string name = "") override; 
        // creates an edge between to given nodes, gets them by ids
        virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) override; 
		// deletes an edge between two nodes, gets them by ids. returns true if successful
		virtual bool deleteEdge(NodeId fromNodeId, NodeId toNodeId);
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
		// factory methods for creating nodes and edges
        virtual Node* createNode(NodeId id, const std::string& name = "") const
        {
            return new Node(id, name);
        }
        
        virtual Node* createNode(const Node* node) const
        {
            if(!node)
				throw std::invalid_argument("Cannot create a node from a null pointer.");
            return new Node(*node);
        }
        
        virtual Edge* createEdge(Node* from, Node* to) const
        {
			if (!from || !to)
				throw std::invalid_argument("Cannot create an edge from null pointers.");
            return new Edge(from, to);
		}

        virtual Edge* createEdge(NodeId fromId, NodeId toId) const
        {
            return new Edge(getNode(fromId), getNode(toId));
        }

		// checks if node id is in bounds of the graphs
        bool nodeIdInBounds(NodeId id) const;
		// checks if edge exists between two nodes, returns true if it does
        bool edgeExists(NodeId fromNodeId, NodeId toNodeId) const;

		// private getters for nodes and edges for internal use
        virtual Node* getNode(NodeId id);
        virtual const Node* getNode(NodeId id) const; // const version
        virtual Edge* getEdge(const EdgeLocator& edgeId);
        virtual const Edge* getEdge(const EdgeLocator& edgeId) const; // const version

        // returns the json representation of the node, used in saveToFile
        virtual nlohmann::json serializeNode(NodeId nodeId) const;
        // creates and returns a new node given json representation of it, used in loadFromFile
        virtual Node* deserializeNode(const nlohmann::json& j) const; 

        // adjacency list contains pairs node : its edges
        using AdjacencyList = std::vector<std::pair<Node*, std::vector<Edge*>>>;
        AdjacencyList m_adjList;

    private:
        // deletes all nodes and edges
        void deleteAdjList(); 

        static constexpr NodeId MAX_NODES = std::numeric_limits<NodeId>::max();
        static constexpr NodeId INIT_NODES = 10;
		
        // TODO: make this dynamic
        static constexpr const char* FILE_NAME = "graph.json";
    };
}