#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "DrawableEdge.h"

namespace graphski
{
	void DrawableGraph::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// draw all the edges first
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

		sf::Color nodeColor = m_nodeColor; // default color
		if(Config::crazyColors)
		{
			nodeColor = sf::Color(rand() % 256, rand() % 256, rand() % 256);
			nodeColor += sf::Color(0xFF050505); // add some white to the color
		}

		auto* newNode = new DrawableNode(id, nodeColor, name);
		newNode->setPosition(position);
		m_adjList.push_back({newNode , {} });

		return id;
	}

	void DrawableGraph::addEdge(uint8_t fromNodeId, uint8_t toNodeId)
	{
		auto* fromPtr = dynamic_cast<DrawableNode*>(m_adjList[fromNodeId].first);
		auto *toPtr = dynamic_cast<DrawableNode*>(m_adjList[toNodeId].first);
		// TODO: check if this edge already exists
		auto* newEdge = new DrawableEdge(fromPtr, toPtr, IDLE_OUTLINE_COLOR);

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

	void DrawableGraph::drawNode(sf::RenderTarget& target, sf::RenderStates states, uint8_t nodeId) const
	{
		// we know only drawable nodes are in a drawable graph
		auto* drawableNode = dynamic_cast<const DrawableNode*>(m_adjList[nodeId].first);
		target.draw(*drawableNode, states);
	}

	void DrawableGraph::drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeId edgeId) const
	{
		// we know only drawable edges are in a drawable graph
		auto* drawableEdge = dynamic_cast<const DrawableEdge*>(getEdge(edgeId));
		target.draw(*drawableEdge, states);
	}
}