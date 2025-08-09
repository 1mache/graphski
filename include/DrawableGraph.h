#pragma once
#include <algorithm>

#include "Graph.h"
#include "SFML/Graphics.hpp"
#include "Utils.h"
#include "DrawableNode.h"
#include "DrawableEdge.h"

namespace graphski
{
	class DrawableGraph : public Graph<DrawableNode, DrawableEdge>, public sf::Drawable
	{
	public:
		DrawableGraph(size_t reserveCount = 0) :
			Graph<DrawableNode, DrawableEdge>(reserveCount)
		{
			m_selectedNodes.reserve(reserveCount);
		}

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		void makeEmpty() override
		{
			Graph<DrawableNode, DrawableEdge>::makeEmpty();
			m_updatedGraph = true;
			m_interactionMode = InteractionMode::None;

			m_selectedNodes.clear();
			m_selectedNodes.reserve(nodeCount());
		}

		NodeId addNode(std::string name = "") override
		{
			NodeId id = Graph<DrawableNode, DrawableEdge>::addNode(name);
			m_adjList[id].first->setColor(getNodeColor()); // set color for the new node

			m_selectedNodes.push_back(false); // extend the selected nodes vector

			m_updatedGraph = true;
			return id;
		}

		NodeId addNode(sf::Vector2f position, std::string name = "")
		{
			NodeId id = addNode(name);
			setNodePosition(id, position); // set the position of the new node
			return id;
		}

		// override: deletes edge if it already exists
		void addEdge(NodeId fromNodeId, NodeId toNodeId) override;

		bool deleteEdge(NodeId fromNodeId, NodeId toNodeId) override
		{
			m_updatedGraph = true;
			return Graph<DrawableNode, DrawableEdge>::deleteEdge(fromNodeId, toNodeId);
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

		void loadFromFile() override
		{
			Graph<DrawableNode, DrawableEdge>::loadFromFile();
			m_updatedGraph = true;
		}

		void toggleSelectNode(NodeId id);

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

		//============================== Interaction modes ==============================
		void startMovingNode(NodeId id)
		{
			m_interactionMode = InteractionMode::Move;
			m_movedId = id;
			markNode(id);
		}

		void moveNode(sf::Vector2f position)
		{
			if (inMoveMode())
			{
				setNodePosition(m_movedId, position);
				m_updatedGraph = true;
			}
			else
				std::cerr << "Trying to call moveNode while not in move mode." << std::endl;
		}

		void stopMovingNode()
		{
			m_interactionMode = InteractionMode::None;
			markNode(m_movedId, false);
		}

		bool inMoveMode() const { return m_interactionMode == InteractionMode::Move; }

		// a node was selected to be source or destination for an edge
		void selectNodeForNewEdge(NodeId id);
		// ==============================================================================
		
		// evenly distributes nodes on screen
		void arrangeNodesEvenly();

		bool isGraphUpdated() const { return m_updatedGraph; }
		void setGraphNotUpdated() { m_updatedGraph = false; }

	private:
		// returns a color for a node (random or not depends on Config)
		sf::Color getNodeColor() const;

		void drawNode(sf::RenderTarget& target, sf::RenderStates states, NodeId nodeId) const;
		void drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeLocator edgeId) const;

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
		enum class InteractionMode
		{
			None,
			Move, // moving a node
			Edge // creating an edge
		};

		sf::Color m_nodeColor{ Config::DEFAULT_NODE_COLOR };

		bool m_updatedGraph = true; // indicates if the graph has been updated with the last events

		InteractionMode m_interactionMode = InteractionMode::None; // current interaction mode
		NodeId			m_movedId  = 0; // id of the node that we're moving
		NodeId			m_fromId = 0, m_toId = 0; // ids of nodes used for edge creation

		std::vector<bool> m_selectedNodes; // bool at index i indicates if the node with id i is selected 
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