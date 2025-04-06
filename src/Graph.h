#pragma once
#include <vector>
#include "Node.h"
#include "Edge.h"

namespace graphski
{
	class Graph
	{
		static constexpr uint8_t MAX_NODES  = UINT8_MAX;
		static constexpr uint8_t INIT_NODES = 10;
		using AdjacencyList = std::vector<std::pair<Node, std::vector<Edge>>>;
		
		AdjacencyList m_adjList;

	public:
		Graph() 
		{
			m_adjList.reserve(INIT_NODES);
		}
	};
}