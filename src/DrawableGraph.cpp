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
			nodeColor += sf::Color(0xFF050505); // add some white to the color for pastel effect
		}

		auto* newNode = new DrawableNode(id, name, nodeColor);
		newNode->setPosition(position);
		m_adjList.push_back({newNode , {} });

		return id;
	}


	std::optional<uint8_t> DrawableGraph::posInNode(sf::Vector2f position)
	{
		// checks if bounds contain position for every node in graph 
		for (uint8_t i = 0; i < m_adjList.size(); i++)
		{
			auto* drawableNode = getNode(i);
			if(drawableNode->getGlobalBounds().contains(position))
				return i;
		}

		return std::nullopt;
	}

	void DrawableGraph::drawNode(sf::RenderTarget& target, sf::RenderStates states, uint8_t nodeId) const
	{
		auto* drawableNode = getNode(nodeId);
		target.draw(*drawableNode, states);
	}

	void DrawableGraph::drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeId edgeId) const
	{
		auto* drawableEdge = getEdge(edgeId);
		target.draw(*drawableEdge, states);
	}
}