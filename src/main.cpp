#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableNode.h"

//constexpr sf::Color NODE_COLOR(0x5F8B4CFF);
//constexpr float     NODE_RADIUS  = 30.f;
//constexpr float     NODE_OUTLINE = 5.f;
//
//void createCircle(std::vector<sf::CircleShape>& circles, sf::Vector2f position)
//{
//    sf::CircleShape newCircle(NODE_RADIUS, 10);
//    newCircle.setFillColor(NODE_COLOR);
//    newCircle.setOutlineThickness(NODE_OUTLINE);
//    newCircle.setOutlineColor(sf::Color::White);
//
//    auto centerOffset = (NODE_RADIUS + NODE_OUTLINE);
//    position -= {centerOffset, centerOffset};
//    newCircle.setPosition(position);
//
//    circles.push_back(newCircle);
//}
//
//void moveCircle(sf::CircleShape& circle, sf::Vector2f position)
//{
//    auto centerOffset = (NODE_RADIUS + NODE_OUTLINE);
//    position -= {centerOffset, centerOffset};
//    circle.setPosition(position);
//}

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
                        nodes.push_back(graphski::DrawableNode(nodeCount, position, txtFont));
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