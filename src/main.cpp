#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

int main()
{
    sf::RenderWindow window (sf::VideoMode({ 800u, 600u }), "CMake SFML Project");
    window.setVerticalSyncEnabled(true);

    std::vector<sf::CircleShape> circles(50);
    uint8_t numOfCircles = 0;

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
                    sf::CircleShape newCircle(30.f);
                    newCircle.setFillColor(sf::Color(0xBFE3B4FF));
                    newCircle.setOutlineThickness(5.f);
                    newCircle.setOutlineColor(sf::Color::White);
                    newCircle.setPosition(sf::Vector2<float>(position) - sf::Vector2f{ 15.f, 15.f });
                    
                    circles[numOfCircles] = newCircle;
                    numOfCircles++;
                    std::cout << circles.size() << std::endl;
                }

            }

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();

        for (auto& circle : circles)
        {
            window.draw(circle);
        }

        window.display();
    }
}