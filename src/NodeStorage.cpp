#include <stdexcept>
#include "NodeStorage.h"

namespace graphski
{
NodeId NodeStorage::addNode(std::unique_ptr<INode> node)
{
    if (size() >= maxNodes())
        throw std::overflow_error("Maximum number of nodes exceeded");
    
    // node is allowed to be nullptr. we just wont count it in nodeCount then
    bool isNull = !node.get();

    NodeId id = getNextMaxId(); // default to next max id
    // if there is a free id reuse it.
    if (!m_freeNodeIds.empty())
    {
        id = m_freeNodeIds.front();
        m_freeNodeIds.pop();
    }

    if(id < m_storage.size())
    {
        // reusing old slot
        m_storage[id] = std::move(node);
    }
    else
    {  
        // adding new slot
        m_storage.push_back(std::move(node));   
    }

    if(!isNull) m_nodeCount++;
    return id;
}

void NodeStorage::insertToEmptySlot(NodeId slotId, std::unique_ptr<INode> insertedNode)
{
    if(getNode(slotId))
        throw std::logic_error("Storage holds node at provided id. Cannot overwrite.");

    m_nodeCount++;
    m_storage[slotId] = std::move(insertedNode);
}

bool NodeStorage::deleteNode(NodeId nodeId)
{
    if(!getNode(nodeId))
        return false; // out of bounds or deleted

    if(nodeId == size() - 1) 
    {
        // if it's the last node, we can just pop it
        m_storage.pop_back();
    }    
    else
    {
        m_storage[nodeId].reset(nullptr);
        m_freeNodeIds.push(nodeId); // add this id to the free list for reuse
    }

    m_nodeCount--;
    return false;
}
} // namespace graphski