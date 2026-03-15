#include <catch2/catch_test_macros.hpp>
#include "Edge.h"
#include "Node.h"

using namespace graphski;

TEST_CASE("Edge Initialization and Connections", "[Edge]") {
    Node fromNode(1, "A");
    Node toNode(2, "B");

    SECTION("Constructor initializes nodes correctly") {
        Edge edge(&fromNode, &toNode);
        
        REQUIRE(edge.getFrom() == &fromNode);
        REQUIRE(edge.getTo() == &toNode);
        REQUIRE_FALSE(edge.isMarked());
    }

    SECTION("Marking edges") {
        Edge edge(&fromNode, &toNode);
        REQUIRE_FALSE(edge.isMarked());

        edge.mark();
        REQUIRE(edge.isMarked());

        edge.mark(false);
        REQUIRE_FALSE(edge.isMarked());
    }

    SECTION("Equality operator") {
        Node otherFromNode(1, "A"); // ID=1
        Node otherToNode(2, "B");   // ID=2
        // NOTE: Edge checks exact pointer equality!
        
        Edge e1(&fromNode, &toNode);
        Edge e2(&fromNode, &toNode);
        Edge e3(&otherFromNode, &otherToNode);

        REQUIRE(e1 == e2);
        REQUIRE_FALSE(e1 == e3); // from/to are different memory addresses.
    }
}
