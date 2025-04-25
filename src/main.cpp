#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableGraph.h"

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    graphski::DrawableGraph graph(sf::Font("fonts/InriaSans.ttf"));
    bool updatedGraph = true;

    bool moveMode = false;
    uint8_t moved_id = 0; // id of the circle inside the vector that we're moving
    bool edgeMode = false;
    uint8_t from_id = 0, to_id = 0;

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
                        moveMode = true;
                        moved_id = id.value(); // remember the moved node`s index 
                        auto* node = graph.getNode(id.value());

                        // TODO: bug if 2 nodes are one on top of another, bottom one also gets marked
                        node->mark(); // mark the moved node
                        updatedGraph = true;
                        std::cout << "Im inside the node " << (int)id.value() << std::endl;
                    }

                    if(!moveMode)
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
                            to_id = id.value();
                            graph.addEdge(from_id, to_id);

                            // done with constructing this edge
                            edgeMode = false;
                            updatedGraph = true;
                        }
                        else
                        {
                            edgeMode = true;
                            from_id = id.value();
                        }
                    }
                }
            }

            if(auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                // disable moveMode when left mouse button is released
                if(mouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    if(moveMode)
                    {
                        moveMode = false;
                        graph.getNode(moved_id)->mark(false); // unmark the moved node
                        updatedGraph = true;
                    }
                }
            }

            if(auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                // if we're moving something right now
                if(moveMode)
                {
                    //TODO: add screen bounds checking
                    sf::Vector2f newPosition(mouseMoved->position);
                    auto* dNode = (graphski::DrawableNode*)(graph.getNode(moved_id));
                    dNode->setPosition(newPosition);
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