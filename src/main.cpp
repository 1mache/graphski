#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "graphics/DrawableGraph.h"
#include "graphics/DrawableNode.h"
#include "graphics/GraphEventHandler.h"
#include "graphics/GraphicsConfig.h"
#include "core/RandomGraphGenerator.h"


int main()
{
    using NodeId = graphski::NodeId;
    using namespace graphski;

    srand((unsigned int)time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ GraphicsConfig::WINDOW_WIDTH , GraphicsConfig::WINDOW_HEIGHT }),
                             "Graphski");

    window.setVerticalSyncEnabled(true);

    DrawableGraph graph;
	GraphEventHandler graphEvents(graph);

	NodeId nodeCount = 16; // number of nodes in the graph
	RandomGraphGenerator generator(graph, nodeCount);
	generator.setProbabilities(RandomGraphGenerator::Distributions::IndexDependent);
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