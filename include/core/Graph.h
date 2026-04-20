#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <memory>
#include <concepts>
#include <type_traits>

#include "nlohmann/json.hpp"
#include "core/IGraph.h"
#include "core/Node.h"
#include "core/NodeStorage.h"

namespace graphski::core
{
    class Graph : public IGraph
    {
    public:
        explicit Graph(size_t reserveCount = 0)
        : m_nodeStorage(reserveCount > NodeStorage::maxNodes() ? 
                                    NodeStorage::maxNodes() : reserveCount)
        {
            m_adjList.reserve(reserveCount);
        }

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

        virtual void clear() override
        {
            m_nodeStorage.clear();
            m_adjList.clear();  
        }
        size_t nodeCount() const override
        {
            return m_nodeStorage.nodeCount();
        }
        size_t edgeCount() const override;
        // node const ref by id
        OptionalNodeConstRef getNode(NodeId id) const override; 
        std::vector<NodeId> getNodeIds() const override;
        // creates a node with empty edges list, returns its unique id
        virtual NodeId addNode(std::string_view name = "") override; 
        // creates an edge between to given nodes, gets them by ids
        virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) override;
        // deletes a node by id, returns true if successful 
        virtual bool deleteNode(NodeId nodeId) override;
		// deletes an edge between two nodes, gets them by ids. returns true if successful
		virtual bool deleteEdge(NodeId fromNodeId, NodeId toNodeId) override; 
        // gets an array of neighbor ids for the given node id
        std::vector<NodeId> getNeighborsOf(NodeId id) const override; 
        // gets a copy of the adjacency list.
        AdjacencyList getAdjacencyList() const override;

    protected:
        template <typename NodeT>
            requires std::derived_from<std::remove_cv_t<NodeT>, INode>
        NodeT* getNodeAs(NodeId id)
        {
            return dynamic_cast<NodeT*>(m_nodeStorage.getNode(id));
        }
        template <typename NodeT>
            requires std::derived_from<std::remove_cv_t<NodeT>, INode>
        const NodeT* getNodeAs(NodeId id) const
        {
            return dynamic_cast<const NodeT*>(m_nodeStorage.getNode(id));
        }
        
		// factory methods for creating nodes
        virtual std::unique_ptr<Node> createNode(std::string_view name = "") const
        {
            return std::make_unique<Node>(name);
        }
        
        virtual std::unique_ptr<Node> createNode(const Node* node) const
        {
            if(!node)
				throw std::invalid_argument("Cannot create a node from a null pointer.");
            return std::make_unique<Node>(*node);
        }
		// checks if edge exists between two nodes, returns true if it does
        bool edgeExists(NodeId fromNodeId, NodeId toNodeId) const;
        
        // cleans up the adjacency list from edges that point to deleted nodes
        void cleanupIncomingEdgesInList(AdjacencyList& adjList) const;
        void ensureCleanAdjList();

        AdjacencyList m_adjList;    
    private:
        NodeStorage m_nodeStorage;
        // used to save unneeded calls to cleanupDeletedDestEdges see impl.
        bool m_adjListNeedsCleanup{false};
        
    // Friends:
        friend void swap(Graph& first, Graph& second) noexcept; // for copy and swap idiom

        friend void transposeGraph(Graph& graph); 
    };
}