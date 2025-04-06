#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

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
                    sf::CircleShape newCircle(30.f,10);
                    newCircle.setFillColor(sf::Color(0x5F8B4CFF));
                    newCircle.setOutlineThickness(5.f);
                    newCircle.setOutlineColor(sf::Color::White);
                    newCircle.setPosition(sf::Vector2<float>(position) - sf::Vector2f{ 17.5f, 17.5f });
                    
                    circles.push_back(newCircle);
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