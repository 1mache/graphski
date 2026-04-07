#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "GraphEventHandler.h"
#include "GraphicsConfig.h"
#include "RandomGraphGenerator.h"


int main()
{
    using NodeId = graphski::NodeId;

    srand((unsigned int)time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ GraphicsConfig::WINDOW_WIDTH , GraphicsConfig::WINDOW_HEIGHT }),
                             "Graphski");

    window.setVerticalSyncEnabled(true);

    graphski::DrawableGraph graph;
	graphski::GraphEventHandler graphEvents(graph);

	NodeId nodeCount = 16; // number of nodes in the graph
	graphski::RandomGraphGenerator generator(graph, nodeCount);
	generator.setProbabilities(graphski::RandomGraphGenerator::Distributions::IndexDependent);
    generator.generate();
    graph.arrangeNodesEvenly();

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            graphEvents.processEvent(event);
        }

        if(graph.isUpdated())
        {
            window.clear();

            window.draw(graph);

            window.display();

            graph.setUpdated(false);
        }
    }
}