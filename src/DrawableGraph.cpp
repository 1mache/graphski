#include "DrawableGraph.h"

namespace graphski
{
	void DrawableGraph::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		for (uint8_t i = 0; i < m_adjList.size(); i++)
		{
			// draw all the edges
			for (uint8_t j = 0; j < m_adjList[i].second.size(); j++)
				drawEdge(target, states, { i, j });
		}

		// draw all the unmoved nodes
		for (uint8_t i = 0; i < m_adjList.size(); i++)
		{
			if (m_moveMode && i == m_movedId)
				continue; // skip the moved node

			drawNode(target, states ,i);
		}
		
		// draw the moved node last
		if(m_moveMode)
			drawNode(target, states, m_movedId);
	}

	uint8_t DrawableGraph::addNode(sf::Vector2f position, std::string name)
	{
		uint8_t id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
		// push back new node with empty edges list
		auto* newNode = new DrawableNode(id, txtFont, name);
		newNode->setPosition(position);
		m_adjList.push_back({newNode , {} });

		return id;
	}

	void DrawableGraph::addEdge(uint8_t fromNodeId, uint8_t toNodeId)
	{
		auto* fromPtr = dynamic_cast<DrawableNode*>(m_adjList[fromNodeId].first);
		auto *toPtr = dynamic_cast<DrawableNode*>(m_adjList[toNodeId].first);

		auto* newEdge = new DrawableEdge(fromPtr, toPtr);

		// increment degrees
		fromPtr->setDOut(fromPtr->getDOut() + 1);
		toPtr->setDIn(toPtr->getDIn() + 1);

		// put new edge into the vector. TODO: sort by id toId inside vector
		m_adjList[fromNodeId].second.push_back(newEdge);
	}

	std::optional<uint8_t> DrawableGraph::posInNode(sf::Vector2f position)
	{
		// checks if bounds contain position for every node in graph 
		for (uint8_t i = 0; i < m_adjList.size(); i++)
		{
			auto* drawableNode = dynamic_cast<DrawableNode*>(m_adjList[i].first);
			if(drawableNode->getGlobalBounds().contains(position))
				return i;
		}

		return std::nullopt;
	}
}