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
			for (NodeId j = 0; j < m_adjList[i].second.size(); j++)
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
		m_selectedNodes.reserve(nodeCount());
	}

	NodeId DrawableGraph::addNode(std::string name)
	{
		NodeId id = Graph::addNode(name);

		m_selectedNodes.push_back(false); // extend the selected nodes vector

		m_updatedGraph = true;
		return id;
	}

	NodeId DrawableGraph::addNode(sf::Vector2f position, std::string name)
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
		if(node) node->toggleSelect();

		if (id >= m_selectedNodes.size()) // should never happen
			throw std::out_of_range("Node id out of bounds in selected nodes vector");

		m_selectedNodes[id] = !m_selectedNodes[id]; // toggle selection state

		m_updatedGraph = true;
	}

	std::optional<NodeId> DrawableGraph::posInNode(sf::Vector2f position)
	{
		// checks if bounds contain position for every node in graph 
		for (const auto& pair: m_adjList)
		{
			NodeId i = pair.first->getId();
			auto* drawableNode = getNode(i);
			if(drawableNode->getGlobalBounds().contains(position))
				return i;
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
		const size_t nodeNum =nodeCount();
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
				setNodePosition(m_adjList[count].first->getId(), {currX, currY});
				currX += dx;
				++count;
			}
			currY += dy;
		}
	}

	DrawableNode* DrawableGraph::createNode(NodeId id, const std::string& name) const
	{
		auto* node = new DrawableNode(id, name);
		node->setColor(getNodeColor()); // set color for the new node
		return node;
	}

	DrawableNode* DrawableGraph::createNode(const Node* node) const
	{
		if (!node)
			throw std::invalid_argument("Cannot create a drawable node from a null pointer.");

		const DrawableNode* drawableNode = dynamic_cast<const DrawableNode*>(node);
		if (drawableNode)
			return new DrawableNode(*drawableNode); // copy the drawable node if possible

		return createNode(node->getId(), node->getName());
	}

	DrawableEdge* DrawableGraph::createEdge(Node* from, Node* to) const
	{
		DrawableNode* fromDrawable = dynamic_cast<DrawableNode*>(from);
		DrawableNode* toDrawable = dynamic_cast<DrawableNode*>(to);
		if (!fromDrawable || !toDrawable)
			throw std::invalid_argument("Non-drawable nodes provided for edge creation.");
		return new DrawableEdge(fromDrawable, toDrawable);
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
		target.draw(*drawableNode, states);
	}

	void DrawableGraph::drawEdge(sf::RenderTarget& target, sf::RenderStates states, const EdgeLocator edgeId) const
	{
		const auto* drawableEdge = getEdge(edgeId);
		target.draw(*drawableEdge, states);
	}

	nlohmann::json DrawableGraph::serializeNode(NodeId nodeId) const
	{
		// call base function which writes all the base data to the json
		auto nodeJson = Graph::serializeNode(nodeId);
		auto* node = getNode(nodeId);

		// add position 
		nodeJson["position"] = { node->getPosition().x, node->getPosition().y };
		return nodeJson;
	}

	DrawableNode* DrawableGraph::deserializeNode(const nlohmann::json& nodeJson) const
	{
		// call base function to get the node without position
		Node* node = Graph::deserializeNode(nodeJson);
		DrawableNode* drawableNode = createNode(node);

		// set the position
		drawableNode->setPosition({ nodeJson["position"][0], nodeJson["position"][1] });

		return drawableNode;
	}
}