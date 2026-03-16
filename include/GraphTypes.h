#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <utility>

namespace graphski
{
	class Node;
	
	// global alias for node id type
	using NodeId = uint16_t;

	// adjacency list contains for each node, a vector of its neighbors (by id)
    using AdjacencyList = std::vector<std::vector<NodeId>>;
}