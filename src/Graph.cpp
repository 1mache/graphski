#include "Graph.h"

namespace graphski
{
	Graph::~Graph()
	{
		for(auto pair: m_adjList)
		{
			auto* nodePtr = pair.first;

			for (auto* edge : pair.second)
			{
				delete edge;
			}
		}
	}

	uint8_t Graph::addNode(std::string name)
	{
		uint8_t id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
		// push back new node with empty edges list
		m_adjList.push_back({ new Node(id, name), {} });

		return id;
	}

	void Graph::addEdge(uint8_t fromNodeId, uint8_t toNodeId)
	{
		Node* fromPtr = m_adjList[fromNodeId].first,
			* toPtr = m_adjList[toNodeId].first;

		Edge* newEdge = new Edge(fromPtr, toPtr);

		// increment degrees
		fromPtr->setDOut(fromPtr->getDOut() + 1);
		toPtr->setDIn(toPtr->getDIn() + 1);

		// put new edge into the vector. TODO: sort by id toId inside vector
		m_adjList[fromNodeId].second.push_back(newEdge);
	}
}