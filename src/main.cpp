#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableNode.h"
#include "DrawableEdge.h"

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    std::vector<graphski::DrawableNode> nodes;
    std::vector<graphski::DrawableEdge> edges;
    nodes.reserve(50);
    edges.reserve(50);

    uint8_t nodeCount = 0;
    bool updatedGraph = true;

    bool moveMode = false;
    uint8_t moved_id = 0; // id of the circle inside the vector that we're moving
    bool edgeMode = false;
    uint8_t from_id = 0, to_id = 0;

    sf::Font txtFont("fonts/InriaSans.ttf");

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f position(mouseButtonPressed->position);

                    for (uint8_t i = 0; i < nodes.size(); i++)
                    {
                        auto& node = nodes[i];
                    
                        // if we clicked inside the circle
                        if(node.getGlobalBounds().contains(position))
                        {
                            moveMode = true;
                            moved_id = i; // remember the moved node`s index 
                            // TODO: bug if 2 nodes are one on top of another, bottom one also gets marked
                            node.mark(); // mark the moved node
                            updatedGraph = true;
                            std::cout << "Im inside the node " << (int)i << std::endl;
                        }
                    }

                    if(!moveMode)
                    {
                        // draw a circle at position
                        auto newNode = graphski::DrawableNode(nodeCount, txtFont);
                        newNode.setPosition(position);
                        nodes.push_back(newNode);
                        nodeCount++;
                        updatedGraph = true;
                    }
                }
                if (mouseButtonPressed->button == sf::Mouse::Button::Right) 
                {
                    sf::Vector2f position(mouseButtonPressed->position);

                    for(uint8_t i = 0; i < nodes.size(); i++)
                    {
                        auto& node = nodes[i];
                        if(node.getGlobalBounds().contains(position))
                        {
                            if(edgeMode)
                            {
                                to_id = i;
                                edges.push_back(graphski::DrawableEdge(&nodes[from_id], &nodes[to_id]));

                                // done with constructing this edge
                                edgeMode = false;
                                updatedGraph = true;
                                std::cout << "Done edging see anything?" << std::endl;
                            }
                            else
                            {
                                edgeMode = true;
                                std::cout << "Started edging rn..." << std::endl;
                                from_id = i;
                            }
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
                        nodes[moved_id].mark(false); // unmark the moved node
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
                    nodes[moved_id].setPosition(newPosition);
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

            for (uint8_t i = 0; i < edges.size(); i++)
            {
                auto& edge = edges[i];
                window.draw(edge);
            }
            for (uint8_t i = 0; i < nodes.size(); i++)
            {
                if (moveMode && i == moved_id)
                    continue;
                
                auto& node = nodes[i];
                window.draw(node);
            }

            if (moveMode)
                window.draw(nodes[moved_id]);

            window.display();

            updatedGraph = false;
        }
    }
}