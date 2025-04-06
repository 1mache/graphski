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

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    std::vector<sf::CircleShape> circles;
    circles.reserve(50);

    bool updatedCircles = true;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2i position = mouseButtonPressed->position;
                    // draw a circle at position
                    createCircle(circles, sf::Vector2f(position));
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