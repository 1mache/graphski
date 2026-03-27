#include "DrawableGraph.h"
#include "DrawableNode.h"

namespace graphski
{
	void DrawableGraph::draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// draw all the edges first
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			for (NodeId j = 0; j < m_adjList[i].size(); j++)
				drawEdge(target, states, EdgeLocator(i,j));
		}

		// draw all the unmoved nodes
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			if ((m_interactionMode == InteractionMode::Move) && i == m_movedId)
				continue; // skip the moved node

			drawNode(target, states ,i);
		}
		
		// draw the moved node last
		if((m_interactionMode == InteractionMode::Move))
			drawNode(target, states, m_movedId);
	}

	void DrawableGraph::makeEmpty()
	{
		Graph::makeEmpty();
		m_updatedGraph = true;
		m_interactionMode = InteractionMode::None;

		m_selectedNodes.clear();
	}

	NodeId DrawableGraph::addNode(std::string_view name)
	{
		NodeId id = Graph::addNode(name);

		m_selectedNodes.push_back(false); // extend the selected nodes vector

		m_updatedGraph = true;
		return id;
	}

	NodeId DrawableGraph::addNode(sf::Vector2f position, std::string_view name)
	{
		NodeId id = addNode(name);
		setNodePosition(id, position); // set the position of the new node
		return id;
	}

	void DrawableGraph::addEdge(NodeId fromNodeId, NodeId toNodeId)
	{
		// if edge exists delete it.
		bool deleteStatus = deleteEdge(fromNodeId, toNodeId);
		if(!deleteStatus) // otherwise create it
			Graph::addEdge(fromNodeId, toNodeId);

		m_updatedGraph = true;
	}

	void DrawableGraph::toggleSelectNode(NodeId id)
	{
		auto* node = getNode(id);
		if(!node) return; // deleted or non existing node 
		
		node->toggleSelect();
		if (id >= m_selectedNodes.size()) // should never happen
			throw std::out_of_range("Node id out of bounds in selected nodes vector");

		m_selectedNodes[id] = !m_selectedNodes[id]; // toggle selection state

		m_updatedGraph = true;
	}

	std::optional<NodeId> DrawableGraph::posInNode(sf::Vector2f position)
	{
		// checks if bounds contain position for every node in graph 
		for (size_t i = 0; i < nodeCount(); ++i)
		{
			auto* drawableNode = getNode(static_cast<NodeId>(i));
			if(drawableNode && drawableNode->getGlobalBounds().contains(position))
				return static_cast<NodeId>(i);
		}

		return std::nullopt;
	}

	void DrawableGraph::startMovingNode(NodeId id)
	{
		m_interactionMode = InteractionMode::Move;
		m_movedId = id;
		markNode(id);
	}

	void DrawableGraph::moveNode(sf::Vector2f position)
	{
		if (inMoveMode())
		{
			setNodePosition(m_movedId, position);
			m_updatedGraph = true;
		}
		else
			std::cerr << "Trying to call moveNode while not in move mode." << std::endl;
	}

	void DrawableGraph::selectNodeForNewEdge(NodeId id)
	{
		if (m_interactionMode == InteractionMode::Edge)
		{
			m_toId = id;
			addEdge(m_fromId, m_toId);
			// done with constructing this edge
			m_interactionMode = InteractionMode::None;
		}
		else
		{
			m_fromId = id;
			m_interactionMode = InteractionMode::Edge;
		}
	}

	void DrawableGraph::arrangeNodesEvenly()
	{
		const size_t nodeNum = nodeCount();
		if(nodeNum == 0) return;
		
		const float fnodeNum = static_cast<float>(nodeNum);
		const float w = static_cast<float>(Config::WINDOW_WIDTH);
		const float h = static_cast<float>(Config::WINDOW_HEIGHT);
		// Calculate grid size (columns and rows)

		float fcols = std::ceil(std::sqrt(fnodeNum * w / h));
		float frows = std::ceil(fnodeNum / fcols);
		size_t cols = static_cast<size_t>(fcols);
		size_t rows = static_cast<size_t>(frows);

		// Calculate spacing
		float dx = w / (fcols + 1);
		float dy = h / (frows + 1);

		// Place points in the grid, center them by offsetting from the borders
		size_t count = 0;

		float currY = dy;
		for (size_t row = 1; row <= rows && count < nodeNum; ++row)
		{
			float currX = dx;
			for (size_t col = 1; col <= cols && count < nodeNum; ++col)
			{
				setNodePosition(static_cast<NodeId>(count), {currX, currY});
				currX += dx;
				++count;
			}
			currY += dy;
		}
	}

	std::unique_ptr<Node> DrawableGraph::createNode(std::string_view name) const
	{
		auto node = std::make_unique<DrawableNode>(name);
		node->setColor(getNodeColor()); // set color for the new node
		return std::move(node);
	}

	std::unique_ptr<Node> DrawableGraph::createNode(const Node* node) const
	{
		if (!node)
			throw std::invalid_argument("Cannot create a drawable node from a null pointer.");

		const DrawableNode* drawableNode = dynamic_cast<const DrawableNode*>(node);
		if (drawableNode)
			return std::make_unique<DrawableNode>(*drawableNode); // copy the drawable node if possible

		return createNode(node->getName());
	}

	sf::Color DrawableGraph::getNodeColor() const
	{
		sf::Color nodeColor = m_nodeColor; // default color
		if (Config::crazyColors)
		{
			do
			{
				nodeColor = sf::Color(static_cast<uint8_t>(rand() % 256), 
									  static_cast<uint8_t>(rand() % 256), 
									  static_cast<uint8_t>(rand() % 256));
				nodeColor += sf::Color(0xFF050505); // add some white to the color for pastel effect
			} while (colorDifference(nodeColor, Config::TEXT_COLOR) < MIN_COLOR_DIFFERENCE);
		}

		return nodeColor;
	}

	void DrawableGraph::drawNode(sf::RenderTarget& target, sf::RenderStates states, NodeId nodeId) const
	{
		const auto* drawableNode = getNode(nodeId);
		if(!drawableNode) return; // deleted or non existing node	
		target.draw(*drawableNode, states);
	}

	void DrawableGraph::drawEdge(sf::RenderTarget& target, sf::RenderStates states, const EdgeLocator edgeId) const
	{
		const auto* from = getNode(edgeId.nodeId);
		const auto* to = getNode(m_adjList[edgeId.nodeId][edgeId.neighborId]);
		// note: actually from should never be null because we delete out edges immediately
		if(!from || !to) return; 

		// TODO: is there a way to cache it? 
		Arrow edge{from->getPosition(), to->getPosition(), EDGE_THICKNESS, Config::IDLE_OUTLINE_COLOR};

		// self edge case
		if (from == to)
		{
			edge.setSelfArrowRadius(DrawableNode::NODE_RADIUS / 2);
			edge.setSelfArrowOffset(DrawableNode::NODE_RADIUS * SELF_EDGE_OFFSET.normalized());
			edge.drawSelfArrow(target, states);
			return;
		}

		// set the offset so that the arrow hits the side of the node
		float offsetExtra = 0.9f; // + shorten it a bit to make it look better
		edge.setHeadOffset(DrawableNode::NODE_RADIUS * offsetExtra);

		edge.draw(target, states);
	}

	nlohmann::json DrawableGraph::serializeNode(NodeId nodeId) const
	{
		// call base function which writes all the base data to the json
		auto nodeJson = Graph::serializeNode(nodeId);
		auto* node = getNode(nodeId);

		// add position 
		if(node)
			nodeJson["position"] = { node->getPosition().x, node->getPosition().y };
		return nodeJson;
	}

	std::unique_ptr<Node> DrawableGraph::deserializeNode(const nlohmann::json& nodeJson) const
	{
		// call base function to get the node without position
		auto nodep = Graph::deserializeNode(nodeJson);
		auto* drawableNodePtr = dynamic_cast<DrawableNode*>(nodep.get());
		if (!drawableNodePtr) throw std::runtime_error("Deserialized node is not a DrawableNode.");

		// set the position
		drawableNodePtr->setPosition({ nodeJson["position"][0], nodeJson["position"][1] });

		return std::unique_ptr<Node>(drawableNodePtr);
	}
}