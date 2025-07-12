#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "GraphEventHandler.h"

constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;

int main()
{
    srand(time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ WINDOW_WIDTH , WINDOW_HEIGHT }), "Graphski");
    window.setVerticalSyncEnabled(true);

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    graphski::DrawableGraph graph;
	graphski::GraphEventHandler eventHandler(graph);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            eventHandler.processEvent(event);
        }

        if(eventHandler.isGraphUpdated())
        {
            window.clear();

            window.draw(graph);

            window.display();

            eventHandler.setGraphNotUpdated();
        }
    }
}