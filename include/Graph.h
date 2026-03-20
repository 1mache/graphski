#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <limits>
#include <algorithm>
#include <memory>

#include "nlohmann/json.hpp"
#include "IGraph.h"
#include "Node.h"

namespace graphski
{
    class Graph : public IGraph
    {
    public:
        explicit Graph(size_t reserveCount = 0);
        
        Graph(const Graph& other);
        Graph& operator=(const Graph& other)
        {
            Graph copy(other); // make a copy using the copy constructor
            swap(*this, copy);
            return *this;
        }

        Graph(Graph&&) = default;        
        Graph& operator=(Graph&&) = default;

        virtual ~Graph() = default;

        // clears the graph
        virtual void makeEmpty() override;
        // how many nodes are there
        size_t nodeCount() const override
        {
            return m_nodes.size();
        }
        // how many edges are there
        size_t edgeCount() const override
        {
            return std::accumulate(m_adjList.begin(), m_adjList.end(), 0u,
                [](size_t sum, const auto& neighbors) { return sum + neighbors.size();});
        }
        // node const ref by id
        const Node& node(NodeId id) const override; 
        // creates a node with empty edges list, returns its unique id
        virtual NodeId addNode(std::string name = "") override; 
        // creates an edge between to given nodes, gets them by ids
        virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) override; 
		// deletes an edge between two nodes, gets them by ids. returns true if successful
		virtual bool deleteEdge(NodeId fromNodeId, NodeId toNodeId); 
        // gets an array of neighbor ids for the given node id
        std::vector<NodeId> getNeighbors(NodeId id) const override; 
        // gets a copy of the adjacency list.
        AdjacencyList getAdjacencyList() const override
        {
            return m_adjList; // return a copy and thats ok! 
        }

    protected:
		// factory methods for creating nodes and edges
        virtual std::unique_ptr<Node> createNode(NodeId id, const std::string& name = "") const
        {
            return std::make_unique<Node>(id, name);
        }
        
        virtual std::unique_ptr<Node> createNode(const Node* node) const
        {
            if(!node)
				throw std::invalid_argument("Cannot create a node from a null pointer.");
            return std::make_unique<Node>(*node);
        }

		// checks if node id is in bounds of the graphs
        bool nodeIdInBounds(NodeId id) const;
		// checks if edge exists between two nodes, returns true if it does
        bool edgeExists(NodeId fromNodeId, NodeId toNodeId) const;

		// private getters for nodes for internal use, raw pointers are non owning.
        virtual Node* getNode(NodeId id);
        virtual const Node* getNode(NodeId id) const; // const version

        // TODO: move these somewhere else
        // returns the json representation of the node, used in saveToFile
        virtual nlohmann::json serializeNode(NodeId nodeId) const;
        // creates and returns a new node given json representation of it, used in loadFromFile
        virtual std::unique_ptr<Node> deserializeNode(const nlohmann::json& j) const; 

        std::vector<std::unique_ptr<Node>> m_nodes;
        AdjacencyList m_adjList;

    private: 
        static constexpr size_t MAX_NODES     = std::numeric_limits<NodeId>::max();
        static constexpr size_t RESERVE_NODES = 10;
        
    // Friends:
        friend void swap(Graph& first, Graph& second) noexcept; // for copy and swap idiom

        friend void transposeGraph(Graph& graph); 
        friend void saveToFile(const Graph& graph, std::string_view fileName);
        friend void loadFromFile(Graph& graph, std::string_view fileName);
    };
}