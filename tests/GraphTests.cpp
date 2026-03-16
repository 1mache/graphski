#include <catch2/catch_test_macros.hpp>
#include "Graph.h"
#include <cstdio> // for std::remove

using namespace graphski;

TEST_CASE("Graph Creation and Basic Operations", "[Graph]") {
    SECTION("Empty Graph properties") {
        Graph g;
        REQUIRE(g.nodeCount() == 0);
        REQUIRE(g.edgeCount() == 0);
        REQUIRE(g.getAdjacencyList().empty());
    }

    SECTION("Adding Nodes") {
        Graph g;
        NodeId id1 = g.addNode("A");
        NodeId id2 = g.addNode(); // Default name
        
        REQUIRE(g.nodeCount() == 2);
        REQUIRE(id1 == 0);
        REQUIRE(id2 == 1);

        auto peek1 = g.peekNode(id1);
        REQUIRE(peek1.name == "A");

        auto peek2 = g.peekNode(id2);
        REQUIRE(peek2.name == "1");
    }

    SECTION("Clearing Graph") {
        Graph g;
        g.addNode();
        g.addNode();
        REQUIRE(g.nodeCount() == 2);
        
        g.makeEmpty();
        REQUIRE(g.nodeCount() == 0);
        REQUIRE(g.edgeCount() == 0);
    }
}

TEST_CASE("Graph Edge Operations", "[Graph][Edge]") {
    Graph g;
    NodeId id1 = g.addNode("A");
    NodeId id2 = g.addNode("B");
    NodeId id3 = g.addNode("C");

    SECTION("Adding Edges") {
        g.addEdge(id1, id2);
        g.addEdge(id2, id3);

        REQUIRE(g.edgeCount() == 2);

        // Check degrees via peekNode
        REQUIRE(g.peekNode(id1).dOut == 1);
        REQUIRE(g.peekNode(id1).dIn == 0);

        REQUIRE(g.peekNode(id2).dOut == 1);
        REQUIRE(g.peekNode(id2).dIn == 1);

        REQUIRE(g.peekNode(id3).dOut == 0);
        REQUIRE(g.peekNode(id3).dIn == 1);
        
        // Check Neighbors
        auto neighbors1 = g.getNeighbors(id1);
        REQUIRE(neighbors1.size() == 1);
        REQUIRE(neighbors1[0] == id2);
    }

    SECTION("Adding Duplicate Edge") {
        g.addEdge(id1, id2);
        g.addEdge(id1, id2); // Should be ignored or handle gracefully
        
        REQUIRE(g.edgeCount() == 1); // Typically handled by simple graph structures
    }

    SECTION("Deleting Edges") {
        g.addEdge(id1, id2);
        REQUIRE(g.edgeCount() == 1);
        
        bool result = g.deleteEdge(id1, id2);
        REQUIRE(result == true);
        REQUIRE(g.edgeCount() == 0);
        REQUIRE(g.getNeighbors(id1).empty());
        
        // Deleting non-existent edge
        bool resultFalse = g.deleteEdge(id1, id3);
        REQUIRE(resultFalse == false);
    }

    SECTION("Graph Transposition") {
        g.addEdge(id1, id2);
        g.addEdge(id2, id3);

        g.transpose();

        REQUIRE(g.edgeCount() == 2);
        
        auto n3 = g.getNeighbors(id3);
        REQUIRE(n3.size() == 1);
        REQUIRE(n3[0] == id2); // Reversed direction
        
        auto n2 = g.getNeighbors(id2);
        REQUIRE(n2.size() == 1);
        REQUIRE(n2[0] == id1); // Reversed direction

        auto n1 = g.getNeighbors(id1);
        REQUIRE(n1.empty()); // Now id1 should have no outgoing edges
        
        // Check new degrees
        REQUIRE(g.peekNode(id1).dIn == 1);
        REQUIRE(g.peekNode(id1).dOut == 0);
        
        REQUIRE(g.peekNode(id2).dIn == 1);
        REQUIRE(g.peekNode(id2).dOut == 1);

        REQUIRE(g.peekNode(id3).dIn == 0);
        REQUIRE(g.peekNode(id3).dOut == 1);
    }
}

TEST_CASE("Graph Serialization", "[Graph][JSON]") {
    Graph g1;
    NodeId idA = g1.addNode("A");
    NodeId idB = g1.addNode("B");
    NodeId idC = g1.addNode("C");

    g1.addEdge(idA, idB);
    g1.addEdge(idB, idC);

    SECTION("Save and Load") {
        // Save to json
        g1.saveToFile();

        // Load into a new graph
        Graph g2;
        g2.loadFromFile();

        // Verify structure matches g1
        REQUIRE(g2.nodeCount() == 3);
        REQUIRE(g2.edgeCount() == 2);

        // Verify nodes 
        REQUIRE(g2.peekNode(idA).name == "A");
        REQUIRE(g2.peekNode(idB).name == "B");
        REQUIRE(g2.peekNode(idC).name == "C");

        // Verify edges match 
        auto neighborsA = g2.getNeighbors(idA);
        REQUIRE(neighborsA.size() == 1);
        REQUIRE(neighborsA[0] == idB);

        auto neighborsB = g2.getNeighbors(idB);
        REQUIRE(neighborsB.size() == 1);
        REQUIRE(neighborsB[0] == idC);

        REQUIRE(g2.getNeighbors(idC).empty());
        
        // Cleanup generated json file testing artifact 
        std::remove("graph.json");
    }
}
