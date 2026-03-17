#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

#include "Graph.h"
#include "GraphTypes.h"

namespace graphski
{
    // transposes graph in place
    void transposeGraph(Graph& graph);
} // namespace graphski