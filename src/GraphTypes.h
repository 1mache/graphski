#pragma once

#include <cstdint>
#include <utility>

namespace graphski
{
	// global alias for node id type
	using NodeId = uint8_t;
	// pair of node id and edges id inside node's edges vector
	using EdgeId = std::pair<NodeId, NodeId>;
}