#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

#include "DrawableGraph.h"
#include "DrawableNode.h"
#include "GraphEventHandler.h"
#include "RandomGraphGenerator.h"

std::vector<sf::Vector2f> generateEvenlyDistributedPoints(size_t n) 
{
    std::vector<sf::Vector2f> points;

    const float w = float(Config::WINDOW_WIDTH);
    const float h = float(Config::WINDOW_HEIGHT);

    // Calculate grid size (columns and rows)
    size_t cols = std::ceil(std::sqrt(n * w / h));
    size_t rows = std::ceil(float(n) / cols);

    // Calculate spacing
    float dx = w / (cols + 1);
    float dy = h / (rows + 1);

    // Place points in the grid, center them by offsetting from the borders
    int count = 0;
    for (size_t row = 1; row <= rows && count < n; ++row) 
    {
        for (size_t col = 1; col <= cols && count < n; ++col) 
        {
            float x = col * dx;
            float y = row * dy;
            points.emplace_back(x, y);
            ++count;
        }
    }
    return points;
}

int main()
{
    using NodeId = graphski::NodeId;

    srand(time(nullptr)); // seed the random number generator

    sf::RenderWindow window (sf::VideoMode({ Config::WINDOW_WIDTH , Config::WINDOW_HEIGHT }),
                            "Graphski");

    window.setVerticalSyncEnabled(true);

    graphski::DrawableGraph graph;
	graphski::GraphEventHandler eventHandler(graph);

	NodeId nodeCount = 64; // number of nodes in the graph
	graphski::RandomGraphGenerator generator(graph, nodeCount);
	generator.setProbabilities(graphski::RandomGraphGenerator::Distributions::iDependent);
    generator.generate();

	auto points = generateEvenlyDistributedPoints(nodeCount);

    auto adjList = graph.getAdjacencyList();
    for (NodeId i = 0; i < adjList.size(); ++i)
    {
        NodeId id = adjList[i].first.id;
		graph.setNodePosition(id, points[i]);
	}

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