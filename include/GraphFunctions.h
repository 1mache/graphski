#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>

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

    // transposes graph in place
    void transposeGraph(Graph& graph);
    
    // serialization and deserialization
    void saveToFile(const Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
    void loadFromFile(Graph& graph, std::string_view fileName = Config::SERIALIZED_FILE_NAME);
} // namespace graphski