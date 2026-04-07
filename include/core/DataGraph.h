#pragma once

#include <stdexcept>
#include <string_view>
#include <utility>
#include <vector>

#include "core/Graph.h"

namespace graphski::core
{
template<typename T>
class DataGraph : public Graph
{
public:
    explicit DataGraph(size_t reserveCount = 0) : Graph(reserveCount) {}
    virtual ~DataGraph() = default;

    DataGraph(const DataGraph& other) : Graph(other), m_data(other.m_data) {}
    DataGraph& operator=(const DataGraph& other);
    DataGraph(DataGraph&&) = default;
    DataGraph& operator=(DataGraph&&) = default;

    NodeId addNode(std::string_view name = "") override;
    // to explicitely give the graph data for this node (recommended)
    NodeId addNode(const T& data, std::string_view name = "");
    NodeId addNode(T&& data, std::string_view name = "");

    const T& getNodeData(NodeId id) const;
    T& getNodeData(NodeId id);
    
private:
    std::vector<T> m_data;
};


template <typename T>
DataGraph<T>& DataGraph<T>::operator=(const DataGraph<T> &other)
{
    DataGraph<T> copy(other); // make a copy using the copy constructor
    swap(*this, copy);
    return *this;
}

template <typename T>
NodeId DataGraph<T>::addNode(std::string_view name)
{
    NodeId id = Graph::addNode(name);        
    m_data.push_back(T{}); // default construct data for this node
    return id;
}

template <typename T>
NodeId DataGraph<T>::addNode(const T &data, std::string_view name)
{  
    NodeId id = Graph::addNode(name);        
    m_data.push_back(data); 
    return id;
}

template <typename T>
NodeId DataGraph<T>::addNode(T &&data, std::string_view name)
{  
    NodeId id = Graph::addNode(name);        
    m_data.push_back(std::move(data)); 
    return id;
}

template <typename T>
const T& DataGraph<T>::getNodeData(NodeId id) const
{
    if (!getNode(id).has_value())
        throw std::out_of_range("Node id out of range or is of deleted node.");
    return m_data[id];
}

template <typename T>
T& DataGraph<T>::getNodeData(NodeId id)
{
    if (!getNode(id).has_value())
        throw std::out_of_range("Node id out of range or is of deleted node.");
    return m_data[id];
}

} // namespace graphski::core
