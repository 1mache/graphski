#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableNode.h"

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    std::vector<graphski::DrawableNode> nodes;
    nodes.reserve(50);

    uint8_t nodeCount = 0;
    bool updatedCircles = true;

    bool moveMode = false;
    size_t moved_id = 0; // id of the circle inside the vector that we're moving

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

                    for (size_t i = 0; i < nodes.size(); i++)
                    {
                        auto& node = nodes[i];
                    
                        // if we clicked inside the circle
                        if(node.getGlobalBounds().contains(position))
                        {
                            moveMode = true;
                            moved_id = i;
                            updatedCircles = true;
                            std::cout << "Im inside the node " << i << std::endl;
                        }
                    }

                    if(!moveMode)
                    {
                        // draw a circle at position
                        auto newNode = graphski::DrawableNode(nodeCount, txtFont);
                        newNode.setPosition(position);
                        nodes.push_back(newNode);
                        nodeCount++;
                        updatedCircles = true;
                    }
                }
            }

            if(auto* mouseButtonReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                // disable moveMode when left mouse button is released
                if(mouseButtonReleased->button == sf::Mouse::Button::Left)
                {
                    moveMode = false;
                }
            }

            if(auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
            {
                // if we're moving something right now
                if(moveMode)
                {
                    sf::Vector2f newPosition(mouseMoved->position);
                    nodes[moved_id].setPosition(newPosition);
                    updatedCircles = true;
                }
            }

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        if(updatedCircles)
        {
            window.clear();

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

            updatedCircles = false;
        }
    }
}