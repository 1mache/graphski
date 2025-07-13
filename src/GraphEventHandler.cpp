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

    void graphski::GraphEventHandler::handleMouseButtonPressed(const sf::Event::MouseButtonPressed& event)
    {
        if (event.button == sf::Mouse::Button::Left)
        {
            sf::Vector2f position(event.position);

			// is there a node where we clicked?
            std::optional<NodeId> id = m_graph.posInNode(position);

            if (id.has_value())
            {
                m_graph.setMoveMode(true);
                m_graph.setMovedId(id.value()); // remember the moved node`s index
                m_graph.markNode(id.value());
            }

            if (!m_graph.inMoveMode())
                m_graph.addNode(position);
        }

        if (event.button == sf::Mouse::Button::Right)
        {
            sf::Vector2f position(event.position);

            std::optional<NodeId> id = m_graph.posInNode(position);
            
            if (id.has_value())
            {
                if (m_graph.inEdgeMode())
                {
                    m_graph.setEdgeToId(id.value());
                    m_graph.addEdge();
                    // done with constructing this edge
                    m_graph.setEdgeMode(false);
                }
                else
                {
					m_graph.setEdgeMode(true);
                    m_graph.setEdgeFromId(id.value());
                }
            }
        }
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