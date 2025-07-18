#pragma once
#include "Graph.h"

namespace graphski
{
	class GraphAlgorithms
	{
		IGraph& m_graph;

	public:
		GraphAlgorithms(IGraph& graph) : m_graph(graph) {}
		GraphAlgorithms(const GraphAlgorithms&) = delete;
		GraphAlgorithms& operator=(const GraphAlgorithms&) = delete;
	};
}