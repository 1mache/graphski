#pragma once

#include "Graph.h"

template<typename T>
class DataGraph : public Graph
{
public:
    explicit DataGraph(size_t reserveCount = 0) : Graph(reserveCount) {}
    virtual ~DataGraph() = default;

    // node const ref by id
    const DataNode<T>& node(NodeId id) const override
    {
        auto* dataNode = dynamic_cast<const DataNode<T>*>(Graph::node(id));
        if (!dataNode)
            throw std::runtime_error("Node not of type DataNode<T> found in DataGraph<T>");
        return *dataNode;
    }

    // shadows the base class version
    NodeId addNode(T data, std::string name = "")
    {  
        if(nodeCount() >= MAX_NODES)
            throw std::overflow_error("Maximum number of nodes exceeded");
        // TODO: change when we can delete nodes
        NodeId id = static_cast<NodeId>(nodeCount());
        
        Graph::insertNode(createNode(id, std::forward<T>(data), name));
        return id;
    }
    
private:
    // factory methods for creating nodes
    std::unique_ptr<Node> createNode(NodeId id, T data, const std::string& name = "") const
    {
        return std::make_unique<DataNode<T>>(id, data, name);
    }
    
    std::unique_ptr<Node> createNode(const Node* node) const
    {
        if(!node)
            throw std::invalid_argument("Cannot create a node from a null pointer.");
        auto* dataNode = dynamic_cast<const DataNode<T>*>(node);
        if (!dataNode)
            throw std::runtime_error("Node not of type DataNode<T> found in DataGraph<T>.");

        return std::make_unique<DataNode<T>>(*dataNode);
    }
};
