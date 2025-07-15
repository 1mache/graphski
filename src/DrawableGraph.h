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

		sf::Color m_nodeColor{Config::DEFAULT_NODE_COLOR};

		bool m_updatedGraph = true; // indicates if the graph has been updated with the last events

		bool m_inMoveMode = false; // if true, we are moving a node
		NodeId m_movedId = 0; // id of the node that we're moving

		bool m_inEdgeMode = false; // if we are currently in edge creation mode
		NodeId m_fromId = 0, m_toId = 0; // ids of nodes used for edge creation

	public:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		void makeEmpty() override
		{
			Graph<DrawableNode, DrawableEdge>::makeEmpty();
			m_updatedGraph = true;
			m_inMoveMode = false;
			m_inEdgeMode = false;
		}

		NodeId addNode(std::string name = "") override;
		NodeId addNode(sf::Vector2f position, std::string name = ""); 

		// edge mode version
		void addEdge()
		{
			if(!m_inEdgeMode)
			{
				std::cout << "Cannot add edge, not in edge mode" << std::endl;
				return;
			}
			addEdge(m_fromId, m_toId);
		}

		void addEdge(NodeId fromNodeId, NodeId toNodeId) override
		{
			Graph<DrawableNode, DrawableEdge>::addEdge(fromNodeId, toNodeId);
			m_updatedGraph = true;
		}
		
		void markNode(NodeId id, bool val = true) override
		{
			Graph<DrawableNode, DrawableEdge>::markNode(id, val);
			m_updatedGraph = true;
		}

		void transpose() override
		{
			Graph<DrawableNode, DrawableEdge>::transpose();
			m_updatedGraph = true;
		}

		// checks if a position is in a certain node, returns its id if it is
		std::optional<NodeId> posInNode(sf::Vector2f position);

		void setNodePosition(NodeId nodeId, sf::Vector2f position)
		{
			auto* node = getNode(nodeId);
			if (node)
			{
				node->setPosition(position);
				m_updatedGraph = true;
			}
		}

		// ================= State management ==================
		bool isGraphUpdated() const { return m_updatedGraph; }
		void setGraphNotUpdated() { m_updatedGraph = false; }

		bool inMoveMode() const { return m_inMoveMode; }
		void setMoveMode(bool val) { m_inMoveMode = val;}

		NodeId getMovedId() const { return m_movedId; }
		void setMovedId(NodeId id) 
		{
			if (nodeIdInBounds(id))
				m_movedId = id; 
		}

		bool inEdgeMode() const { return m_inEdgeMode; }
		void setEdgeMode(bool val) { m_inEdgeMode = val; }

		void setEdgeFromId(NodeId id)
		{
			if (nodeIdInBounds(id))
				m_fromId = id;
		}

		void setEdgeToId(NodeId id)
		{
			if (nodeIdInBounds(id))
				m_toId = id;
		}

		void loadFromFile() override
		{
			Graph<DrawableNode, DrawableEdge>::loadFromFile();
			m_updatedGraph = true;
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