#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <utility>
#include <limits>
#include <iostream>

#include "INode.h"
#include "GraphTypes.h"

namespace graphski
{
// Sparse storage for nodes.
class NodeStorage
{
    public:
        NodeStorage(size_t reserveCount = 0)
        {
            m_storage.reserve(reserveCount);
        }

        // we dont know how to deep copy the specific nodes. 
        // so its up to the holder of the storage to manage copying
        NodeStorage(const NodeStorage& other) = delete;
        NodeStorage& operator=(const NodeStorage& other) = delete; 

        NodeStorage(NodeStorage&&) = default;
        NodeStorage& operator=(NodeStorage&&) = default;

        // how big is the storage
        size_t size() const
        {
            return m_storage.size();
        }
        // how many nodes are currently stored
        size_t nodeCount() const
        {
            return m_nodeCount;
        }

        void reserve(size_t newSize)
        {
            if (newSize > maxNodes())
                newSize = maxNodes();

            m_storage.reserve(newSize);
        }
        void resize(size_t newSize)
        {
            if (newSize > maxNodes())
                newSize = maxNodes();

            m_storage.resize(newSize);
        }

        void clear()
        {   
            m_storage.clear();
            m_nodeCount = 0;        
            m_freeNodeIds = std::queue<NodeId>();
        }

        bool nodeIdInBounds(NodeId id) const
        {
            if (id >= m_storage.size())
            {
                std::cerr << "Node id out of bounds: " << (int)id << std::endl;
                return false;
            }
            return true;
        }

        // add node to storage, returns access id
        NodeId addNode(std::unique_ptr<INode> node);
        void insertToEmptySlot(NodeId slotId, std::unique_ptr<INode> insertedNode);
        // deletes a node by id, returns true if successful 
        bool deleteNode(NodeId nodeId);

        // getters for nodes, raw pointers are non owning.
        INode* getNode(NodeId id)
        {
            if (!nodeIdInBounds(id))
                return nullptr;
            return m_storage[id].get();
        }
        const INode* getNode(NodeId id) const // const version
        {
            if (!nodeIdInBounds(id))
                return nullptr;
            return m_storage[id].get();
        }
    
        auto begin() { return m_storage.begin(); }
        auto end() { return m_storage.end(); }
        auto begin() const { return m_storage.begin(); }
        auto end() const { return m_storage.end(); }

        static constexpr size_t maxNodes()
        {
            return std::numeric_limits<NodeId>::max();
        }
        
        friend void swap(NodeStorage& first, NodeStorage& second) noexcept
        {
            using std::swap;
            swap(first.m_storage, second.m_storage);
            swap(first.m_freeNodeIds, second.m_freeNodeIds);
            swap(first.m_nodeCount, second.m_nodeCount);
        }
    private:
        // get NOT reused id
        NodeId getNextMaxId()
        {
            return static_cast<NodeId>(size());
        }

        std::vector<std::unique_ptr<INode>> m_storage;
        std::queue<NodeId> m_freeNodeIds; // queue of free node ids for reuse
        size_t m_nodeCount{0};
};
    
} // namespace graphski
