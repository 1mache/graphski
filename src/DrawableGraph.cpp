#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "DrawableEdge.h"

namespace graphski
{
	void DrawableGraph::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// draw all the edges first
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			// draw all the edges
			for (NodeId j = 0; j < m_adjList[i].second.size(); j++)
				drawEdge(target, states, { i, j });
		}

		// draw all the unmoved nodes
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			if (m_inMoveMode && i == m_movedId)
				continue; // skip the moved node

			drawNode(target, states ,i);
		}
		
		// draw the moved node last
		if(m_inMoveMode)
			drawNode(target, states, m_movedId);
	}

	NodeId DrawableGraph::addNode(sf::Vector2f position, std::string name)
	{
		NodeId id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
		// push back new node with empty edges list

		auto* newNode = new DrawableNode(id, name, getNodeColor());
		newNode->setPosition(position);
		m_adjList.push_back({newNode , {} });

		return id;
	}


	std::optional<NodeId> DrawableGraph::posInNode(sf::Vector2f position)
	{
		// checks if bounds contain position for every node in graph 
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			auto* drawableNode = m_adjList[i].first;
			if(drawableNode->getGlobalBounds().contains(position))
				return i;
		}

		return std::nullopt;
	}

	sf::Color DrawableGraph::getNodeColor() const
	{
		sf::Color nodeColor = m_nodeColor; // default color
		if (Config::crazyColors)
		{
			do
			{
				nodeColor = sf::Color(rand() % 256, rand() % 256, rand() % 256);
				nodeColor += sf::Color(0xFF050505); // add some white to the color for pastel effect
			} while (colorDifference(nodeColor, Config::TEXT_COLOR) < MIN_COLOR_DIFFERENCE);
		}

		return nodeColor;
	}

	void DrawableGraph::drawNode(sf::RenderTarget& target, sf::RenderStates states, NodeId nodeId) const
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