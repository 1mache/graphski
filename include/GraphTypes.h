#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <utility>

namespace graphski
{
	// global alias for node id type
	using NodeId = uint16_t;
	// pair of node id and edges id inside node's edges vector
	using EdgeId = std::pair<NodeId, NodeId>;

	struct NodePeek
	{
		NodeId id;
		std::string name;
	};

	// adjacency list type for peeking into the graph
	// pairs for nodeId : neighbors ids
	using AdjacencyListPeek = std::vector<std::pair<NodePeek, std::vector<NodeId>>>;
}