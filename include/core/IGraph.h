#pragma once
#include <vector>
#include "core/GraphTypes.h"

namespace graphski
{
	class INode;
	// General base class for graph traversal
	class IGraph
	{
	public:
		virtual ~IGraph() = default;

		virtual void clear() = 0;

		virtual size_t nodeCount() const = 0;
		virtual size_t edgeCount() const = 0;

		virtual OptionalNodeConstRef getNode(NodeId id) const = 0;
		virtual std::vector<NodeId>  getNodeIds() const = 0;

		virtual NodeId addNode(std::string_view name = "") = 0;
		virtual void   addEdge(NodeId fromNodeId, NodeId toNodeId) = 0;

		virtual bool deleteNode(NodeId id) = 0;
		virtual bool deleteEdge(NodeId fromNodeId, NodeId toNodeId) = 0;

		virtual std::vector<NodeId> getNeighborsOf(NodeId id) const = 0;
		virtual AdjacencyList 		getAdjacencyList() const = 0;
	};
}