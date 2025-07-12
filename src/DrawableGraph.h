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
		NodeId m_movedId = 0; // id of the node that we're moving

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
		NodeId addNode(sf::Vector2f position, std::string name = "");

		// checks if a position is in a certain node, returns its id if it is
		std::optional<NodeId> posInNode(sf::Vector2f position);

		// getter and setter for movedId
		NodeId getMovedNodeId() const { return m_movedId; }
		void setMovedNodeId(NodeId value) { m_movedId = value; }
		
		// getter and setter for moveMode
		bool isInMoveMode() const { return m_moveMode; }
		void setMoveMode(bool value) { m_moveMode = value; }

		void setNodePosition(NodeId nodeId, sf::Vector2f position)
		{
			auto* node = getNode(nodeId);
			if(node)
				node->setPosition(position);
			else
				std::cerr << "Node ID out of range: " << static_cast<int>(nodeId) << std::endl;
		}

	private:
		// returns a color for a node (random or not depends on Config)
		sf::Color getNodeColor() const;

		void drawNode(sf::RenderTarget& target, sf::RenderStates states, NodeId nodeId) const;
		void drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeId edgeId) const;

		// override function to serialize a drawable node
		nlohmann::json serializeNode(const DrawableNode* node) const override
		{
			// call base function 
			auto nodeJson = Graph<DrawableNode, DrawableEdge>::serializeNode(node);
			// add position 
			nodeJson["position"] = { node->getPosition().x, node->getPosition().y };
			return nodeJson;
		}

		// override function to deserialize a drawable node
		DrawableNode* deserializeNode(const nlohmann::json& nodeJson) const override
		{
			// call base function to get the node without position
			auto* node = Graph<DrawableNode, DrawableEdge>::deserializeNode(nodeJson);
			// set the position
			node->setPosition({nodeJson["position"][0], nodeJson["position"][1]});
			node->setColor(getNodeColor()); // set color
			return node;
		}

	private:
		// helper fucntion: calculates the difference between two colors
		constexpr uint32_t colorDifference(const sf::Color& a, const sf::Color& b) const
		{
			return Utils::absDiff(a.toInteger(), b.toInteger());
		}

		// min color difference (for random color selection)
		static constexpr uint32_t MIN_COLOR_DIFFERENCE = Utils::absDiff(Config::DEFAULT_NODE_COLOR.toInteger(),
																		Config::TEXT_COLOR.toInteger());
	};
}