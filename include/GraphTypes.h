#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <utility>

namespace graphski
{
	// global alias for node id type
	using NodeId = uint16_t;

	struct NodePeek
	{
		std::string_view name;
		NodeId		id;
		NodeId		dIn, dOut; // in and out degrees
	};

	// adjacency list type for peeking into the graph
	// pairs for nodeId : neighbors ids
	using AdjacencyListPeek = std::vector<std::pair<NodePeek, std::vector<NodeId>>>;
}