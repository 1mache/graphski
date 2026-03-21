#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>

#include "Graph.h"
#include "Node.h"
#include "GraphTypes.h"
#include "Config.h"

namespace graphski
{
    std::unique_ptr<Node> createNode(std::string_view name);

    void swap(Graph& first, Graph& second) noexcept;

    // transposes graph in place
    void transposeGraph(Graph& graph);
    
    // serialization and deserialization
    void saveToFile(const Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
    void loadFromFile(Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
} // namespace graphski