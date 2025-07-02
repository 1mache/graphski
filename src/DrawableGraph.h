#pragma once
#include "Graph.h"
#include "SFML/Graphics.hpp"
#include "Utils.h"
#include "DrawableNode.h"
#include "DrawableEdge.h"

namespace graphski
{
	class DrawableGraph : public Graph<DrawableNode, DrawableEdge>, public sf::Drawable
	{
		bool m_moveMode = false; // if true, we are moving a node
		uint8_t m_movedId = 0; // id of the node that we're moving

		sf::Color m_nodeColor{Config::DEFAULT_NODE_COLOR};

	public:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// clear the graph
		void makeEmpty() override
		{
			m_moveMode = false;
			m_movedId = 0;
			Graph::makeEmpty();
		}

		// creates a node with empty edges list at given position, returns its unique id
		// note: blocks Graph::addNode because we cant create a drawable node without position
		uint8_t addNode(sf::Vector2f position, std::string name = "");

		// checks if a position is in a certain node, returns its id if it is
		std::optional<uint8_t> posInNode(sf::Vector2f position);

		// getter and setter for movedId
		uint8_t getMovedNodeId() const { return m_movedId; }
		void setMovedNodeId(uint8_t value) { m_movedId = value; }
		
		// getter and setter for moveMode
		bool isInMoveMode() const { return m_moveMode; }
		void setMoveMode(bool value) { m_moveMode = value; }

		void setNodePosition(uint8_t nodeId, sf::Vector2f position)
		{
			auto* node = getNode(nodeId);
			if(node)
				node->setPosition(position);
			else
				std::cerr << "Node ID out of range: " << static_cast<int>(nodeId) << std::endl;
		}

	private:
		void drawNode(sf::RenderTarget& target, sf::RenderStates states, uint8_t nodeId) const;
		void drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeId edgeId) const;

	private:
		// helper fucntion: calculates the difference between two colors
		constexpr uint32_t colorDifference(const sf::Color& a, const sf::Color& b) const
		{
			return Utils::absDiff(a.toInteger(), b.toInteger());
		}

		// min color difference between text and background color (for random color selection)
		static constexpr uint32_t MIN_COLOR_DIFFERENCE = Utils::absDiff(Config::DEFAULT_NODE_COLOR.toInteger(),
																		Config::TEXT_COLOR.toInteger());
	};
}