#include <catch2/catch_test_macros.hpp>
#include "core/Graph.h"

TEST_CASE("Add node and add edge", "[Graph]")
{
    graphski::Graph graph;
    
    SECTION("Add node")
    {
        auto node1 = graph.addNode("Node1");
        REQUIRE(graph.nodeCount() == 1);
        CHECK(graph.peekNode(node1).name == "Node1");
    }

    SECTION("Add node 2")
    {
        auto node1 = graph.addNode("Node1");
        auto node2 = graph.addNode("Node2");
        auto node3 = graph.addNode("Node3");

        REQUIRE(graph.nodeCount() == 3);
        CHECK(graph.peekNode(node1).name == "Node1");
        CHECK(graph.peekNode(node2).name == "Node2");
        CHECK(graph.peekNode(node3).name == "Node3");
    }

    SECTION("Add edge")
    {
        auto node1 = graph.addNode("Node1");
        auto node2 = graph.addNode("Node2");

        graph.addEdge(node1, node2);

        REQUIRE(graph.edgeCount() == 1);
        CHECK(graph.getNeighborsOf(node1) == std::vector<graphski::NodeId>{node2});
        CHECK(graph.getNeighborsOf(node2).empty());
    }

    SECTION("Add edge 2")
    {
        auto node1 = graph.addNode("Node1");
        auto node2 = graph.addNode("Node2");
        auto node3 = graph.addNode("Node3");

        graph.addEdge(node1, node2);
        graph.addEdge(node1, node3);
        graph.addEdge(node2, node3);

        REQUIRE(graph.edgeCount() == 3);
        CHECK(graph.getNeighborsOf(node1) == std::vector<graphski::NodeId>{node2, node3});
        CHECK(graph.getNeighborsOf(node2) == std::vector<graphski::NodeId>{node3});
        CHECK(graph.getNeighborsOf(node3).empty());
    }
}

TEST_CASE("Adjacency list", "[Graph]")
{
    graphski::Graph graph;

    auto node1 = graph.addNode("Node1");
    auto node2 = graph.addNode("Node2");
    auto node3 = graph.addNode("Node3");

    graph.addEdge(node1, node2);
    graph.addEdge(node1, node3);
    graph.addEdge(node2, node3);

    auto adjList = graph.getAdjacencyList();

    REQUIRE(adjList.size() == 3);
    CHECK(adjList[node1].first.id == node1);
    CHECK(adjList[node1].second.size() == 2); // 2 neighbors
    CHECK(adjList[node1].second == std::vector<graphski::NodeId>{node2, node3});

    CHECK(adjList[node2].first.id == node2);
    CHECK(adjList[node2].second.size() == 1);
    CHECK(adjList[node2].second == std::vector<graphski::NodeId>{node3});

    CHECK(adjList[node3].first.id == node3);
    CHECK(adjList[node3].second.empty());
}

TEST_CASE("Make empty", "[Graph]")
{
    graphski::Graph graph;

    auto node1 = graph.addNode("Node1");
    auto node2 = graph.addNode("Node2");
    graph.addEdge(node1, node2);

    REQUIRE(graph.nodeCount() == 2);
    REQUIRE(graph.edgeCount() == 1);

    graph.clear();

    REQUIRE(graph.nodeCount() == 0);
    REQUIRE(graph.edgeCount() == 0);
}

TEST_CASE("Delete edge", "[Graph]")
{
    graphski::Graph graph;

    auto node1 = graph.addNode("Node1");
    auto node2 = graph.addNode("Node2");
    auto node3 = graph.addNode("Node3");

    graph.addEdge(node1, node2);
    graph.addEdge(node1, node3);
    graph.addEdge(node2, node3);

    REQUIRE(graph.edgeCount() == 3);

    CHECK(graph.deleteEdge(node1, node2) == true);
    REQUIRE(graph.edgeCount() == 2);
    CHECK(graph.getNeighborsOf(node1) == std::vector<graphski::NodeId>{node3});
    CHECK(graph.getNeighborsOf(node2) == std::vector<graphski::NodeId>{node3});
    CHECK(graph.getNeighborsOf(node3).empty());

    CHECK(graph.deleteEdge(node1, node2) == false); // edge already deleted
}
