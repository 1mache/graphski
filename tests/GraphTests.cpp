#include <catch2/catch_test_macros.hpp>
#include "Graph.h"
#include "GraphFunctions.h"
#include <cstdio> // for std::remove
#include <stdexcept>

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
        NodeId id2 = g.addNode(); // default name should be id as string
        
        REQUIRE(g.nodeCount() == 2);
        REQUIRE(id1 == 0);
        REQUIRE(id2 == 1);

        auto node1 = g.node(id1);
        REQUIRE(node1.has_value());
        REQUIRE(node1.value().get().getName() == "A");

        auto node2 = g.node(id2);
        REQUIRE(node2.has_value());
        REQUIRE(node2.value().get().getName() == "1");
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

TEST_CASE("Graph Operations", "[Graph]") {
    Graph g;
    NodeId id1 = g.addNode("A");
    NodeId id2 = g.addNode("B");
    NodeId id3 = g.addNode("C");

    SECTION("Adding Edges") {
        g.addEdge(id1, id2);
        g.addEdge(id2, id3);

        REQUIRE(g.edgeCount() == 2);

        // Check degrees via peekNode
        auto node1 = g.node(id1);
        auto node2 = g.node(id2);
        auto node3 = g.node(id3);
        REQUIRE(node1.has_value());
        REQUIRE(node2.has_value());
        REQUIRE(node3.has_value());

        REQUIRE(node1.value().get().getDegOut() == 1);
        REQUIRE(node1.value().get().getDegIn() == 0);

        REQUIRE(node2.value().get().getDegOut() == 1);
        REQUIRE(node2.value().get().getDegIn() == 1);

        REQUIRE(node3.value().get().getDegOut() == 0);
        REQUIRE(node3.value().get().getDegIn() == 1);
        
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

    SECTION("Deleting Nodes") {
        g.addEdge(id1, id2);
        g.addEdge(id2, id3);
        
        bool result = g.deleteNode(id2);
        REQUIRE(result == true);
        REQUIRE(g.nodeCount() == 2);
        REQUIRE(g.edgeCount() == 0); // Edges involving deleted node should be removed
        REQUIRE_FALSE(g.node(id2).has_value());

        // Any edge operation using a deleted node id should fail.
        REQUIRE_THROWS_AS(g.addEdge(id1, id2), std::invalid_argument);
        REQUIRE_THROWS_AS(g.deleteEdge(id1, id2), std::invalid_argument);

        // Next insertion should reuse the freed node id.
        NodeId reusedId = g.addNode("D");
        REQUIRE(reusedId == id2);
        REQUIRE(g.nodeCount() == 3);
        auto reusedNode = g.node(reusedId);
        REQUIRE(reusedNode.has_value());
        REQUIRE(reusedNode.value().get().getName() == "D");
        
        // Deleting non-existent node
        bool resultFalse = g.deleteNode(999);
        REQUIRE(resultFalse == false);
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
        Graph empty{};
        REQUIRE_NOTHROW(transposeGraph(empty));

        g.addEdge(id1, id2);
        g.addEdge(id2, id3);

        transposeGraph(g);

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
        auto node1 = g.node(id1);
        auto node2 = g.node(id2);
        auto node3 = g.node(id3);
        REQUIRE(node1.has_value());
        REQUIRE(node2.has_value());
        REQUIRE(node3.has_value());

        REQUIRE(node1.value().get().getDegIn() == 1);
        REQUIRE(node1.value().get().getDegOut() == 0);
        
        REQUIRE(node2.value().get().getDegIn() == 1);
        REQUIRE(node2.value().get().getDegOut() == 1);

        REQUIRE(node3.value().get().getDegIn() == 0);
        REQUIRE(node3.value().get().getDegOut() == 1);
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
        std::string_view filename = "serialized_test.json";
        // Save to json
        saveToFile(g1, filename);

        // Load into a new graph
        Graph g2;
        loadFromFile(g2, filename);

        // Verify structure matches g1
        REQUIRE(g2.nodeCount() == 3);
        REQUIRE(g2.edgeCount() == 2);

        // Verify nodes 
        auto nodeA = g2.node(idA);
        auto nodeB = g2.node(idB);
        auto nodeC = g2.node(idC);
        REQUIRE(nodeA.has_value());
        REQUIRE(nodeB.has_value());
        REQUIRE(nodeC.has_value());
        REQUIRE(nodeA.value().get().getName() == "A");
        REQUIRE(nodeB.value().get().getName() == "B");
        REQUIRE(nodeC.value().get().getName() == "C");

        // Verify edges match 
        auto neighborsA = g2.getNeighbors(idA);
        REQUIRE(neighborsA.size() == 1);
        REQUIRE(neighborsA[0] == idB);

        auto neighborsB = g2.getNeighbors(idB);
        REQUIRE(neighborsB.size() == 1);
        REQUIRE(neighborsB[0] == idC);

        REQUIRE(g2.getNeighbors(idC).empty());
        
        // Cleanup generated json file testing artifact 
        std::remove(filename.data());
    }
}

TEST_CASE("Graph Rule of Five", "[Graph][RuleOf5]") {
    SECTION("Constructors create valid graph") {
        Graph gDefault;
        Graph gReserved(32);

        REQUIRE(gDefault.nodeCount() == 0);
        REQUIRE(gDefault.edgeCount() == 0);
        REQUIRE(gReserved.nodeCount() == 0);
        REQUIRE(gReserved.edgeCount() == 0);
    }

    SECTION("Copy constructor performs deep copy") {
        Graph source;
        NodeId a = source.addNode("A");
        NodeId b = source.addNode("B");
        source.addEdge(a, b);

        Graph copy(source);

        REQUIRE(copy.nodeCount() == source.nodeCount());
        REQUIRE(copy.edgeCount() == source.edgeCount());
        auto copiedNodeA = copy.node(a);
        REQUIRE(copiedNodeA.has_value());
        REQUIRE(copiedNodeA.value().get().getName() == "A");
        REQUIRE(copy.getNeighbors(a).size() == 1);
        REQUIRE(copy.getNeighbors(a)[0] == b);

        // Mutate source to verify the copy is independent.
        REQUIRE(source.node(a).has_value());
        source.addNode("C");
        source.deleteEdge(a, b);

        REQUIRE(copy.nodeCount() == 2);
        REQUIRE(copy.edgeCount() == 1);
        REQUIRE(copy.getNeighbors(a).size() == 1);
        REQUIRE(copy.getNeighbors(a)[0] == b);
    }

    SECTION("Move constructor transfers graph state") {
        Graph source;
        NodeId a = source.addNode("A");
        NodeId b = source.addNode("B");
        source.addEdge(a, b);

        Graph moved(std::move(source));

        REQUIRE(moved.nodeCount() == 2);
        REQUIRE(moved.edgeCount() == 1);
        auto movedNodeA = moved.node(a);
        REQUIRE(movedNodeA.has_value());
        REQUIRE(movedNodeA.value().get().getName() == "A");
        REQUIRE(moved.getNeighbors(a).size() == 1);
        REQUIRE(moved.getNeighbors(a)[0] == b);

        // Moved-from object must remain valid and usable.
        source.makeEmpty();
        NodeId x = source.addNode("X");
        REQUIRE(x == 0);
        REQUIRE(source.nodeCount() == 1);
    }

    SECTION("Copy assignment copies full graph state") {
        Graph source;
        NodeId a = source.addNode("A");
        NodeId b = source.addNode("B");
        source.addEdge(a, b);

        Graph target;
        target.addNode("Old");

        target = source;

        REQUIRE(target.nodeCount() == 2);
        REQUIRE(target.edgeCount() == 1);
        auto targetNodeA = target.node(a);
        REQUIRE(targetNodeA.has_value());
        REQUIRE(targetNodeA.value().get().getName() == "A");
        REQUIRE(target.getNeighbors(a).size() == 1);
        REQUIRE(target.getNeighbors(a)[0] == b);

        source.deleteEdge(a, b);
        REQUIRE(source.edgeCount() == 0);
        REQUIRE(target.edgeCount() == 1);
    }

    SECTION("Move assignment transfers full graph state") {
        Graph source;
        NodeId a = source.addNode("A");
        NodeId b = source.addNode("B");
        source.addEdge(a, b);

        Graph target;
        target.addNode("Old");
        target.addNode("Data");

        target = std::move(source);

        REQUIRE(target.nodeCount() == 2);
        REQUIRE(target.edgeCount() == 1);
        auto targetNodeA = target.node(a);
        REQUIRE(targetNodeA.has_value());
        REQUIRE(targetNodeA.value().get().getName() == "A");
        REQUIRE(target.getNeighbors(a).size() == 1);
        REQUIRE(target.getNeighbors(a)[0] == b);

        // Moved-from object must remain valid and usable.
        source.makeEmpty();
        source.addNode("AfterMove");
        REQUIRE(source.nodeCount() == 1);
    }
}
