#include "GraphEventHandler.h"

namespace graphski
{
    void GraphEventHandler::processEvent(const std::optional<sf::Event>& event)
    {
        if (!event.has_value())
            return;

        if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            handleMouseButtonPressed(*mouseButtonPressed);

        if (auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            handleMouseButtonReleased(*mouseButtonReleased);

        if (auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            handleMouseMoved(*mouseMoved);

        if (auto* key = event->getIf<sf::Event::KeyPressed>())
            handleKeyPressed(*key);
    }

    void GraphEventHandler::handleMouseButtonPressed(const sf::Event::MouseButtonPressed& event)
    {
        sf::Vector2f position(event.position);
        if (event.button == sf::Mouse::Button::Left)
			handleLMBPressed(position);

        if (event.button == sf::Mouse::Button::Right)
			handleRMBPressed(position);
    }

    void GraphEventHandler::handleRMBPressed(sf::Vector2f& position)
    {
        std::optional<NodeId> id = m_graph.posInNode(position);

        if (id.has_value())
			m_graph.selectNodeForNewEdge(id.value());
    }

    void GraphEventHandler::handleLMBPressed(sf::Vector2f& position)
    {
        // is there a node where we clicked?
        std::optional<NodeId> id = m_graph.posInNode(position);

        if (!id.has_value())
        {
            m_graph.addNode(position);
            return;
        }

        // double click logic
        auto timeSinceLastClick = m_clickClock.getElapsedTime() - m_lastClick;
        if (isDoubleClick(timeSinceLastClick))
        {
            handleNodeDoubleClick(id.value());
            // restart clock and reset lastClick
            m_clickClock.restart();
            m_lastClick = sf::milliseconds(0);
            return;
        }
        else m_lastClick = m_clickClock.getElapsedTime();

        // moveMode
        m_graph.setMoveMode(true);
        m_graph.setMovedId(id.value()); // remember the moved node`s index
        m_graph.markNode(id.value());
    }

    void GraphEventHandler::handleNodeDoubleClick(NodeId nodeId)
    {
		std::cout << "Node " << nodeId << " double clicked.\n";

        m_graph.toggleSelectNode(nodeId);
    }

    void GraphEventHandler::handleMouseButtonReleased(const sf::Event::MouseButtonReleased& event)
    {
        // disable moveMode when left mouse button is released
        if (event.button == sf::Mouse::Button::Left)
        {
            if (m_graph.inMoveMode())
            {
				m_graph.setMoveMode(false);
                m_graph.markNode(m_graph.getMovedId(), false); // unmark the moved node
            }
        }
    }

    void GraphEventHandler::handleMouseMoved(const sf::Event::MouseMoved& event)
    {
        // if we're moving something right now
        if (m_graph.inMoveMode())
        {
            sf::Vector2f newPosition(event.position);
            //TODO: add more sophisticated screen bounds checking
            if (posInBounds(newPosition))
                m_graph.setNodePosition(m_graph.getMovedId(), newPosition);
        }
    }

    void GraphEventHandler::handleKeyPressed(const sf::Event::KeyPressed& event)
    {
        switch (event.code)
        {
        case WRITE_TO_FILE_KEY:
            m_graph.saveToFile();
            break;
        case LOAD_FROM_FILE_KEY:
            m_graph.loadFromFile();
			break;

        case CLEAR_GRAPH_KEY:
            m_graph.makeEmpty();
			break;

        case TRANSPOSE_KEY:
			m_graph.transpose();
			break;

        default:
			break;
        }
    }
}