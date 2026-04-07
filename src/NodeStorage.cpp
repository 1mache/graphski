#include <stdexcept>
#include "NodeStorage.h"

namespace graphski
{
NodeId NodeStorage::addNode(std::unique_ptr<INode>&& node)
{
    if (size() >= maxNodes())
        throw std::overflow_error("Maximum number of nodes exceeded");
    
    if(!node)
        throw std::invalid_argument("Got addNode as an argument for addNode");

    NodeId id = getNextMaxId(); // default to next max id
    // if there is a free id reuse it.
    if (!m_freeNodeIds.empty())
    {
        id = m_freeNodeIds.back();
        m_freeNodeIds.pop_back();
        // reusing old slot
        m_storage[id] = std::move(node);
    }
    else
    {  
        // adding new slot
        m_storage.push_back(std::move(node));   
    }

    ++m_nodeCount;
    return id;
}

bool NodeStorage::deleteNode(NodeId nodeId)
{
    if(!getNode(nodeId))
        return false; // out of bounds or deleted

    if(nodeId == size() - 1) 
    {
        // if it's the last node, we can just pop it
        m_storage.pop_back();
        if(!m_storage.empty() && m_storage.back() == nullptr)
        {
            while(m_storage.back() == nullptr)
                m_storage.pop_back(); // pop nullptrs if any in the back

            // remove any freed ids that are out of bounds now
            std::erase_if(m_freeNodeIds, [this](NodeId id){ return id >= size(); });
        }
    }    
    else
    {
        m_storage[nodeId].reset(nullptr);
        m_freeNodeIds.push_back(nodeId); // add this id to the free list for reuse
    }

    --m_nodeCount;
    return true;
}
} // namespace graphski