#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

constexpr sf::Color NODE_COLOR(0x5F8B4CFF);
constexpr float     NODE_RADIUS  = 30.f;
constexpr float     NODE_OUTLINE = 5.f;

void createCircle(std::vector<sf::CircleShape>& circles, sf::Vector2f position)
{
    sf::CircleShape newCircle(NODE_RADIUS, 10);
    newCircle.setFillColor(NODE_COLOR);
    newCircle.setOutlineThickness(NODE_OUTLINE);
    newCircle.setOutlineColor(sf::Color::White);

    auto centerOffset = (NODE_RADIUS + NODE_OUTLINE);
    position -= {centerOffset, centerOffset};
    newCircle.setPosition(position);

    circles.push_back(newCircle);
}

void moveCircle(sf::CircleShape& circle, sf::Vector2f position)
{
    auto centerOffset = (NODE_RADIUS + NODE_OUTLINE);
    position -= {centerOffset, centerOffset};
    circle.setPosition(position);
}

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 6;

    std::vector<sf::CircleShape> circles;
    circles.reserve(50);

    bool updatedCircles = true;

    bool moveMode = false;
    size_t moved_id = 0; // id of the circle inside the vector that we're moving
    
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f position(mouseButtonPressed->position);

                    for (size_t i = 0; i < circles.size(); i++)
                    {
                        auto& circle = circles[i];
                    
                        // if we clicked inside the circle
                        if(circle.getGlobalBounds().contains(position))
                        {
                            moveMode = true;
                            moved_id = i;
                            std::cout << "Im inside the circle " << i << std::endl;
                        }
                    }

                    if(!moveMode)
                    {
                        // draw a circle at position
                        createCircle(circles, position);
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
                    moveCircle(circles[moved_id], newPosition);
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

            for (auto& circle : circles)
            {
                window.draw(circle);
            }

            window.display();

            updatedCircles = false;
        }
    }
}