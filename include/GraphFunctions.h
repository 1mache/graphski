#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <functional>

#include "Graph.h"
#include "DataGraph.h"
#include "Node.h"
#include "GraphTypes.h"
#include "Config.h"

namespace graphski
{    
    void swap(Graph& first, Graph& second) noexcept;
    template<typename T>
    void swap(DataGraph<T>& first, DataGraph<T>& second) noexcept
    {
        swap(static_cast<Graph&>(first), static_cast<Graph&>(second)); // swap base class parts
        std::swap(first.m_data, second.m_data); // swap the data vector as well
    }

    // TODO: try to make these non friend.
    // transposes graph in place
    void transposeGraph(Graph& graph);
    
    // serialization and deserialization:

    // returns the json representation of the INode generic node
    nlohmann::json serializeINode(const INode& node, NodeId nodeId);
    
    // for graph-type specific node serialization.
    using NodeSerializer = std::function<nlohmann::json(const INode& node, NodeId nodeId)>;

    void saveToFile(const IGraph& graph,
                    const NodeSerializer& nodeSerializer,
                    std::string_view fileName = Config::SERIALIZED_FILE_NAME);
    void loadFromFile(Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
} // namespace graphski