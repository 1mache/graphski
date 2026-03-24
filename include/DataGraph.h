#pragma once

#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include "Graph.h"

namespace graphski
{
template<typename T>
class DataGraph : public Graph
{
public:
    explicit DataGraph(size_t reserveCount = 0) : Graph(reserveCount) {}
    virtual ~DataGraph() = default;

    NodeId addNode(std::string_view name = "") override
    {
        NodeId id = Graph::addNode(name);        
        m_data.push_back(T{}); // default construct data for this node
        return id;
    }
    // have to give the graph data for this node
    NodeId addNode(const T& data, std::string_view name = "")
    {  
        NodeId id = Graph::addNode(name);        
        m_data.push_back(data); 
        return id;
    }
    NodeId addNode(T&& data, std::string_view name = "")
    {  
        NodeId id = Graph::addNode(name);        
        m_data.push_back(std::move(data)); 
        return id;
    }

    const T& getNodeData(NodeId id) const
    {
        if (id >= m_data.size())
            throw std::out_of_range("Node id out of range for data retrieval.");
        return m_data[id];
    }

    T retrieveNodeData(NodeId id)
    {
        if (id >= m_data.size())
            throw std::out_of_range("Node id out of range for data retrieval.");
        return m_data[id];
    }
    
private:
    std::vector<T> m_data;
};
} // namespace graphski
