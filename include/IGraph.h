#pragma once
#include <vector>
#include "GraphTypes.h"

namespace graphski
{
	class INode;
	// General base class for graphs
	class IGraph
	{
	public:
		virtual ~IGraph() = default;

		virtual void makeEmpty() = 0;

		virtual size_t nodeCount() const = 0;
		virtual size_t edgeCount() const = 0;

		virtual const INode& node(NodeId id) const = 0;

		virtual NodeId addNode(std::string name = "") = 0;
		virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) = 0;

		virtual std::vector<NodeId> getNeighbors(NodeId id) const = 0;
		virtual AdjacencyList getAdjacencyList() const = 0;
	};
}