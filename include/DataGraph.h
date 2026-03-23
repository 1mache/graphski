#pragma once

#include "Graph.h"

template<typename T>
class DataGraph : public Graph
{
public:
    explicit DataGraph(size_t reserveCount = 0) : Graph(reserveCount) {}
    virtual ~DataGraph() = default;

    NodeId addNode(std::string_view name = "")
    {
        throw std::runtime_error("addNode with name only is not implemented for DataGraph");
    }
    // have to give the graph data for this node
    NodeId addNode(T data, std::string_view name = "")
    {  
        NodeId id = Graph::addNode(name);        
        m_data.push_back(data);
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
        return std::move(m_data[id]);
    }
    
private:
    std::vector<T> m_data;
};
