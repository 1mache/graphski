#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "GraphEventHandler.h"
#include "RandomGraphGenerator.h"


int main()
{
    using NodeId = graphski::NodeId;

    srand(time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ Config::WINDOW_WIDTH , Config::WINDOW_HEIGHT }),
                             "Graphski");

    window.setVerticalSyncEnabled(true);

    graphski::DrawableGraph graph;
	graphski::GraphEventHandler eventHandler(graph);

	NodeId nodeCount = 16; // number of nodes in the graph
	graphski::RandomGraphGenerator generator(graph, nodeCount);
	generator.setProbabilities(graphski::RandomGraphGenerator::Distributions::IndexDependent);
    generator.generate();
    graph.distributePoints();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            eventHandler.processEvent(event);
        }

        if(graph.isGraphUpdated())
        {
            window.clear();

            window.draw(graph);

            window.display();

            graph.setGraphNotUpdated();
        }
    }
}