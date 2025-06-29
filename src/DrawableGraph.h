#pragma once
#include "Graph.h"
#include "SFML/Graphics.hpp"

namespace graphski
{
	class DrawableGraph : public Graph, public sf::Drawable
	{
		// pair of node id and edges id inside node's edges vector
		using EdgeId = std::pair<uint8_t, uint8_t>;

		bool m_moveMode = false; // if true, we are moving a node
		uint8_t m_movedId = 0; // id of the node that we're moving

		sf::Color m_nodeColor{DEFAULT_NODE_COLOR};

	public:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// creates a node with empty edges list at given position, returns its unique id
		// note: blocks Graph::addNode because we cant create a drawable node without position
		uint8_t addNode(sf::Vector2f position, std::string name = "");

		// creates an edge between to given nodes, gets them by ids
		void addEdge(uint8_t fromNodeId, uint8_t toNodeId) override;

		// checks if a position is in a certain node, returns its id if it is
		std::optional<uint8_t> posInNode(sf::Vector2f position);

		// getter and setter for movedId
		uint8_t getMovedNodeId() const { return m_movedId; }
		void setMovedNodeId(uint8_t value) { m_movedId = value; }
		
		// getter and setter for moveMode
		bool isInMoveMode() const { return m_moveMode; }
		void setMoveMode(bool value) { m_moveMode = value; }

	private:
		Edge* getEdge(EdgeId edgeId) const
		{
			// goes to the node and then to the edge inside the node edge vector
			return (m_adjList[edgeId.first].second[edgeId.second]);
		}

		void drawNode(sf::RenderTarget& target, sf::RenderStates states, uint8_t nodeId) const;
		void drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeId edgeId) const;

	public:
		static constexpr sf::Color DEFAULT_NODE_COLOR{ 0xFFB200FF };
		static constexpr sf::Color IDLE_OUTLINE_COLOR{ sf::Color::White};
		static constexpr sf::Color MARKED_COLOR      { 0xD91656FF };
		static constexpr sf::Color SELECTED_COLOR    { 0xD91656FF };
		static constexpr sf::Color TEXT_COLOR        { sf::Color::White };
	};
}