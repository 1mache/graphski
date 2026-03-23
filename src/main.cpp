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

    srand((unsigned int)time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ Config::WINDOW_WIDTH , Config::WINDOW_HEIGHT }),
                             "Graphski");

    window.setVerticalSyncEnabled(true);

    graphski::DrawableGraph graph;
	graphski::GraphEventHandler graphEvents(graph);

	NodeId nodeCount = 16; // number of nodes in the graph
    for(NodeId i = 0; i < nodeCount; ++i)
        graph.addNode();
    
	graphski::RandomGraphGenerator generator(graph);
    generator.generate(graphski::RandomGraphGenerator::Distributions::IndexDependent);
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