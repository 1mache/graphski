#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include "json.hpp"
#include "Node.h"
#include "Edge.h"

namespace graphski
{
	class Graph
	{
	protected:
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
		virtual ~Graph();

		// how many nodes are there
		uint8_t nodeCount() const { return (uint8_t)m_adjList.size(); }
		// for given node how many edges does it have
		uint8_t edgeCount(uint8_t nodeId) const { return (uint8_t)m_adjList[nodeId].second.size(); }

		// returns the copy of the adjacency list (TODO: find better alternative to "peek in")
		AdjacencyList getAdjListCopy() const { return m_adjList;}

		// creates a node with empty edges list, returns its unique id
		virtual uint8_t addNode(std::string name = "");

		// creates an edge between to given nodes, gets them by ids
		virtual void addEdge(uint8_t fromNodeId, uint8_t toNodeId);

		// returns the node pointer by id 
		// TODO: this is not great
		Node* getNode(uint8_t nodeId) { return m_adjList[nodeId].first; }
		
		void saveToFile() const;
		// TODO: in DrawableGraph we need to draw this somehow
		void loadFromFile();

	private: // constants

		static constexpr uint8_t MAX_NODES = UINT8_MAX;
		static constexpr uint8_t INIT_NODES = 10;
		static constexpr const char* FILE_NAME = "graph.json";
	};

}