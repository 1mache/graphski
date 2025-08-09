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

	void DrawableGraph::addEdge(NodeId fromNodeId, NodeId toNodeId)
	{
		// if edge exists delete it.
		bool deleteStatus = deleteEdge(m_fromId, m_toId);
		if(!deleteStatus) // otherwise create it
			Graph<DrawableNode, DrawableEdge>::addEdge(fromNodeId, toNodeId);

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
		for (NodeId i = 0; i < m_adjList.size(); i++)
		{
			auto* drawableNode = m_adjList[i].first;
			if(drawableNode->getGlobalBounds().contains(position))
				return i;
		}

		return std::nullopt;
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
		size_t nodeNum = nodeCount();

		const float w = float(Config::WINDOW_WIDTH);
		const float h = float(Config::WINDOW_HEIGHT);

		// Calculate grid size (columns and rows)
		size_t cols = std::ceil(std::sqrt(nodeNum * w / h));
		size_t rows = std::ceil(float(nodeNum) / cols);

		// Calculate spacing
		float dx = w / (cols + 1);
		float dy = h / (rows + 1);

		// Place points in the grid, center them by offsetting from the borders
		int count = 0;
		for (size_t row = 1; row <= rows && count < nodeNum; ++row)
		{
			for (size_t col = 1; col <= cols && count < nodeNum; ++col)
			{
				float x = col * dx;
				float y = row * dy;

				setNodePosition(m_adjList[count].first->getId(), {x, y});
				++count;
			}
		}
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
		const auto* drawableNode = getNode(nodeId);
		target.draw(*drawableNode, states);
	}

	void DrawableGraph::drawEdge(sf::RenderTarget& target, sf::RenderStates states, const EdgeLocator edgeId) const
	{
		const auto* drawableEdge = getEdge(edgeId);
		target.draw(*drawableEdge, states);
	}
}