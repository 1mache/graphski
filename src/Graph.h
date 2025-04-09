#pragma once
#include <vector>
#include "Node.h"
#include "Edge.h"

namespace graphski
{
	class Graph
	{
		// adjacency list contains pairs node : its edges
		// TODO: maybe have edges get ids of the nodes they need instead of pointers, then it could me on the stack
		using AdjacencyList = std::vector<std::pair<Node*, std::vector<Edge*>>>;
		
		AdjacencyList m_adjList;

	public:
		Graph() 
		{
			m_adjList.reserve(INIT_NODES);
		}

		Graph(Graph&) = delete;
		Graph& operator=(Graph&) = delete;
		~Graph();

		// how many nodes are there
		uint8_t nodeCount() { return static_cast<uint8_t>(m_adjList.size()); }

		// returns the copy of the adjacency list (TODO: find better alternative to "peek in")
		AdjacencyList getAdjListCopy() { return m_adjList;}

		// creates a node with empty edges list, returns its unique id
		uint8_t addNode(std::string name = "");

		// creates an edge between to given nodes, gets them by ids
		void addEdge(uint8_t fromNodeId, uint8_t toNodeId);

	private: // constants

		static constexpr uint8_t MAX_NODES = UINT8_MAX;
		static constexpr uint8_t INIT_NODES = 10;
	};

}