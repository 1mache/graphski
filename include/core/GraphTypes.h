#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <optional>

namespace graphski
{
	class INode;

	// alias for node id type
	using NodeId = uint16_t;
	// alias for degree type, max degree = max id since id is ordered 
	using Degree = NodeId; 

	using OptionalNodeConstRef = std::optional<std::reference_wrapper<const INode>>;
	
	// adjacency list contains for each node, a vector of its neighbors (by id)
    using AdjacencyList = std::vector<std::vector<NodeId>>;

	struct EdgeLocator
	{
		EdgeLocator(NodeId _nodeId, size_t _neighborId) :
			nodeId(_nodeId), neighborId(_neighborId) {}
		NodeId nodeId;
		size_t neighborId; // index of the neighbor in the node's edges vector
	};
}