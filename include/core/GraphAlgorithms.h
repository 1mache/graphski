#pragma once
#include "core/Graph.h"

namespace graphski::core
{
	class GraphAlgorithms
	{
		[[maybe_unused]] IGraph& m_graph;

	public:
		GraphAlgorithms(IGraph& graph) : m_graph(graph) {}
		GraphAlgorithms(const GraphAlgorithms&) = delete;
		GraphAlgorithms& operator=(const GraphAlgorithms&) = delete;
	};
}