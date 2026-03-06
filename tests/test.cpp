#include <catch2/catch_test_macros.hpp>
#include "Graph.h"

graphski::Graph graph;

auto node1 = graph.addNode("Node1");
auto node2 = graph.addNode("Node2");
auto node3 = graph.addNode("Node3");

TEST_CASE("Add node", "[Graph]")
{
    REQUIRE(graph.nodeCount() == 3);
    CHECK(graph.peekNode(node1).name == "Node1");
    CHECK(graph.peekNode(node2 ).name == "Node2");
    CHECK(graph.peekNode(node3).name == "Node3");
}

TEST_CASE("Add edge", "[Graph]")
{
    graph.addEdge(node1, node2);
    graph.addEdge(node1, node3);
    graph.addEdge(node2, node3);

    REQUIRE(graph.edgeCount() == 3);
    CHECK(graph.getNeighbors(node1) == std::vector<graphski::NodeId>{node2, node3});
    CHECK(graph.getNeighbors(node2) == std::vector<graphski::NodeId>{node3});
    CHECK(graph.getNeighbors(node3).empty());    
}