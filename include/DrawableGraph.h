#pragma once
#include <algorithm>

#include "SFML/Graphics.hpp"

#include "Graph.h"
#include "Utils.h"
#include "DrawableNode.h"
#include "Arrow.h"

namespace graphski
{
	class DrawableGraph : public Graph, public sf::Drawable
	{
	public:
		explicit DrawableGraph(size_t reserveCount = 0) :
			Graph(reserveCount)
		{}

		DrawableGraph(const DrawableGraph&) = delete;
		DrawableGraph& operator=(const DrawableGraph&) = delete;
		DrawableGraph(DrawableGraph&&) noexcept = delete;
		DrawableGraph& operator=(DrawableGraph&&) noexcept = delete; 

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		void clear() override;

		NodeId addNode(std::string_view name = "") override;
		NodeId addNode(sf::Vector2f position, std::string_view name = "");

		// override functionality: deletes edge if it already exists
		void addEdge(NodeId fromNodeId, NodeId toNodeId) override;

		bool deleteEdge(NodeId fromNodeId, NodeId toNodeId) override
		{
			m_updatedGraph = true;
			return Graph::deleteEdge(fromNodeId, toNodeId);
		}
		
		void markNode(NodeId id, bool val = true)
		{
			auto* node = getNodeAs<DrawableNode>(id);
			if (node)
				node->mark(val);
			m_updatedGraph = true;
		}

		void toggleSelectNode(NodeId id);

		// checks if a position is in a certain node, returns its id if it is
		std::optional<NodeId> posInNode(sf::Vector2f position);

		void setNodePosition(NodeId nodeId, sf::Vector2f position)
		{
			auto* node = getNodeAs<DrawableNode>(nodeId);
			if (node)
			{
				node->setPosition(position);
				m_updatedGraph = true;
			}
		}

		//============================== Interaction modes ==============================
		// when you want to move a node
		void startMovingNode(NodeId id);
		// used to change the moved node position
		void moveNode(sf::Vector2f position);
		// stops moving a node, resets the interaction mode
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
		
		bool isUpdated() const { return m_updatedGraph; }
		void setUpdated(bool value) { m_updatedGraph = value; }

	private:
		// factory methods for creating nodes and edges. overrides the base Graph methods
		std::unique_ptr<Node> createNode(std::string_view name = "") const override;
		
		std::unique_ptr<Node> createNode(const Node* node) const override;

		// returns a color for a node (random or not depends on Config)
		sf::Color getNodeColor() const;

		void drawNode(sf::RenderTarget& target, sf::RenderStates states, NodeId nodeId) const;
		void drawEdge(sf::RenderTarget& target, sf::RenderStates states, EdgeLocator edgeId) const;

	private:
		enum class InteractionMode
		{
			None,
			Move, // moving a node
			Edge // creating an edge
		};

		sf::Color m_nodeColor{ Config::DEFAULT_NODE_COLOR };

		static constexpr float EDGE_THICKNESS = 5.f;

		// position offset of the self edge circle relative to the node center
		static constexpr sf::Vector2f SELF_EDGE_OFFSET{ -1.f, -1.f };

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