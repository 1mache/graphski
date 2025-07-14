#pragma once
#include "DrawableGraph.h"
#include "Config.h"

namespace graphski 
{
    class GraphEventHandler
    {
        DrawableGraph& m_graph;

    public:
        GraphEventHandler(graphski::DrawableGraph& graph)
            : m_graph(graph)
        {}
		GraphEventHandler(const GraphEventHandler&) = delete;
		GraphEventHandler& operator=(const GraphEventHandler&) = delete;   

        void processEvent(const std::optional<sf::Event>& event);
    
    private:
        void handleMouseButtonPressed(const sf::Event::MouseButtonPressed& event);
        void handleMouseButtonReleased(const sf::Event::MouseButtonReleased& event);
        void handleMouseMoved(const sf::Event::MouseMoved& event);
        void handleKeyPressed(const sf::Event::KeyPressed& event);

        bool posInBounds(sf::Vector2f position)
        {
            return (0 <= position.x && position.x <= Config::WINDOW_WIDTH) &&
                (0 <= position.y && position.y <= Config::WINDOW_HEIGHT);
        }

    private:

        // key constants
        static constexpr sf::Keyboard::Key WRITE_TO_FILE_KEY  = sf::Keyboard::Key::W;
        static constexpr sf::Keyboard::Key LOAD_FROM_FILE_KEY = sf::Keyboard::Key::L;
        static constexpr sf::Keyboard::Key CLEAR_GRAPH_KEY    = sf::Keyboard::Key::C;
        static constexpr sf::Keyboard::Key TRANSPOSE_KEY      = sf::Keyboard::Key::T;
    };
}