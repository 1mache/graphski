#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "GraphEventHandler.h"

int main()
{
    srand(time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ Config::WINDOW_WIDTH , Config::WINDOW_HEIGHT }),
                            "Graphski");

    window.setVerticalSyncEnabled(true);

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