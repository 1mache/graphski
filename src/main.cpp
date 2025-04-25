#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableGraph.h"

constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;

bool posInBounds(sf::Vector2f position)
{
    return (0 <= position.x && position.x <= WINDOW_WIDTH) &&
           (0 <= position.y && position.y <= WINDOW_HEIGHT);

}

int main()
{
    sf::RenderWindow window (sf::VideoMode({ WINDOW_WIDTH , WINDOW_HEIGHT }), "Graphski");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    graphski::DrawableGraph graph(sf::Font("fonts/InriaSans.ttf"));
    bool updatedGraph = true;

    bool edgeMode = false;
    uint8_t fromId = 0, toId = 0;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f position(mouseButtonPressed->position);

                    std::optional<uint8_t> id = graph.posInNode(position);

                    if(id.has_value())
                    {
                        graph.setMoveMode(true);
                        graph.setMovedNodeId(id.value()); // remember the moved node`s index
                        auto* node = graph.getNode(id.value());

                        // TODO: bug if 2 nodes are one on top of another, bottom one also gets marked
                        node->mark(); // mark the moved node
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
                    
                    std::optional<uint8_t> id = graph.posInNode(position);
                    if (id.has_value())
                    {
                        auto* node = graph.getNode(id.value());
                        if (edgeMode)
                        {
                            toId = id.value();
                            graph.addEdge(fromId, toId);

                            // done with constructing this edge
                            edgeMode = false;
                            updatedGraph = true;
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
                        graph.getNode(graph.getMovedNodeId())->mark(false); // unmark the moved node
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
                        auto* dNode = (graphski::DrawableNode*)(graph.getNode(graph.getMovedNodeId()));
                        dNode->setPosition(newPosition);
                        updatedGraph = true;
                    }
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