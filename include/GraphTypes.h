#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <utility>

namespace graphski
{
	class Node;
	class Edge;
	
	// global alias for node id type
	using NodeId = uint16_t;

	// adjacency list contains pairs node : its edges
    using AdjacencyList = std::vector<std::pair<Node*, std::vector<Edge*>>>;

	using AdjacencyListView = std::vector<std::vector<NodeId>>;
}