#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableGraph.h"
#include "DrawableNode.h"

constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;

bool posInBounds(sf::Vector2f position)
{
    return (0 <= position.x && position.x <= WINDOW_WIDTH) &&
           (0 <= position.y && position.y <= WINDOW_HEIGHT);

}

int main()
{
    constexpr sf::Keyboard::Key WRITE_TO_FILE_KEY  = sf::Keyboard::Key::W;
    constexpr sf::Keyboard::Key LOAD_FROM_FILE_KEY = sf::Keyboard::Key::L;
    constexpr sf::Keyboard::Key CLEAR_GRAPH_KEY    = sf::Keyboard::Key::C;
    constexpr sf::Keyboard::Key TRANSPOSE_KEY      = sf::Keyboard::Key::T;

    srand(time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ WINDOW_WIDTH , WINDOW_HEIGHT }), "Graphski");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

	using NodeId = graphski::NodeId;
    graphski::DrawableGraph graph;
    bool updatedGraph = true;

    bool edgeMode = false;
    NodeId fromId = 0, toId = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f position(mouseButtonPressed->position);

                    std::optional<NodeId> id = graph.posInNode(position);

                    if(id.has_value())
                    {
                        graph.setMoveMode(true);
                        graph.setMovedNodeId(id.value()); // remember the moved node`s index
                        graph.markNode(id.value());
                        updatedGraph = true;
                        std::cout << "Im inside the node " << (int)id.value() << std::endl;
                    }

                    if(!graph.isInMoveMode())
                    {
                        graph.addNode(position);
                        updatedGraph = true;
                    }
                }
                if (mouseButtonPressed->button == sf::Mouse::Button::Right) 
                {
                    sf::Vector2f position(mouseButtonPressed->position);
                    
                    std::optional<NodeId> id = graph.posInNode(position);
                    if (id.has_value())
                    {
                        if (edgeMode)
                        {
                            toId = id.value();
                            if(toId != fromId) // if its node from node to itself
                            {
                                graph.addEdge(fromId, toId);
                                // done with constructing this edge
                                edgeMode = false;
                                updatedGraph = true;

                            }
                        }
                        else
                        {
                            edgeMode = true;
                            fromId = id.value();
                        }
                    }
                }
            }

            if(auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                // disable moveMode when left mouse button is released
                if(mouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    if(graph.isInMoveMode())
                    {
                        graph.setMoveMode(false);
                        graph.markNode(graph.getMovedNodeId(), false); // unmark the moved node
                        updatedGraph = true;
                    }
                }
            }

            if(auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                // if we're moving something right now
                if(graph.isInMoveMode())
                {
                    //TODO: add more sophisticated screen bounds checking
                    sf::Vector2f newPosition(mouseMoved->position);
                    if(posInBounds(newPosition))
                    {
						graph.setNodePosition(graph.getMovedNodeId(), newPosition);
                        updatedGraph = true;
                    }
                }
            }

            if (auto* key =  event->getIf<sf::Event::KeyPressed>())
            {
				if (key->code == WRITE_TO_FILE_KEY)
					graph.saveToFile();
                if (key->code == LOAD_FROM_FILE_KEY)
                {
                    graph.loadFromFile();
					updatedGraph = true;
                }

                if (key->code == CLEAR_GRAPH_KEY)
                {
                    graph.makeEmpty();
                    // TODO: move both into the DrawableGraph so it hadles all that
                    updatedGraph = true;
                    edgeMode = false;
                }

                if (key->code == TRANSPOSE_KEY)
                {
                    graph.transpose();
                    updatedGraph = true;
                }
            }

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        if(updatedGraph)
        {
            window.clear();

            window.draw(graph);

            window.display();

            updatedGraph = false;
        }
    }
}