#pragma once
#include <vector>
#include "GraphTypes.h"

namespace graphski
{
	// General base class for graphs
	class IGraph
	{
	public:
		virtual ~IGraph() = default;

		virtual void makeEmpty() = 0;

		virtual NodeId nodeCount() const = 0;
		virtual size_t edgeCount(NodeId id) const = 0;

		virtual NodeId addNode(std::string name = "") = 0;
		virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) = 0;

		virtual void markNode(NodeId id, bool val = true) = 0;

		virtual std::vector<NodeId> getNeighbors(NodeId id) const = 0;
		virtual AdjacencyListPeek getAdjacencyList() const = 0;
	};
}