#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#include "Graph.h"
#include "GraphTypes.h"
#include "Config.h"
#include "nlohmann/json.hpp"

namespace graphski
{
    // transposes graph in place
    void transposeGraph(Graph& graph);
    
    // serialization and deserialization
    void saveToFile(const Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
    void loadFromFile(Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
} // namespace graphski