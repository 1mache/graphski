#pragma once
#include "DrawableGraph.h"

namespace graphski 
{
    class GraphEventHandler
    {
        graphski::DrawableGraph& m_graph;

		bool m_updatedGraph = true; // indicates if the graph has been updated with the last events

		bool m_inEdgeMode = false; // if we are currently in edge creation mode
		NodeId fromId = 0, toId = 0; // ids of nodes used for edge creation

    public:
        GraphEventHandler(graphski::DrawableGraph& graph)
            : m_graph(graph)
        {}

        bool isGraphUpdated() const { return m_updatedGraph; } 

		// the state is set to true only by event processing functions
		// from outside you can only set it to false
        void setGraphNotUpdated() { m_updatedGraph = false;}

        void processEvent(const std::optional<sf::Event>& event);
    
    private:
        void handleMouseButtonPressed(const sf::Event::MouseButtonPressed& event);
        void handleMouseButtonReleased(const sf::Event::MouseButtonReleased& event);
        void handleMouseMoved(const sf::Event::MouseMoved& event);
        void handleKeyPressed(const sf::Event::KeyPressed& event);

        bool posInBounds(sf::Vector2f position)
        {
            return (0 <= position.x && position.x <= WINDOW_WIDTH) &&
                (0 <= position.y && position.y <= WINDOW_HEIGHT);
        }

    private:

        // key constants
        static constexpr sf::Keyboard::Key WRITE_TO_FILE_KEY  = sf::Keyboard::Key::W;
        static constexpr sf::Keyboard::Key LOAD_FROM_FILE_KEY = sf::Keyboard::Key::L;
        static constexpr sf::Keyboard::Key CLEAR_GRAPH_KEY    = sf::Keyboard::Key::C;
        static constexpr sf::Keyboard::Key TRANSPOSE_KEY      = sf::Keyboard::Key::T;

        static constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;
    };
}