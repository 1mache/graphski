#include <catch2/catch_test_macros.hpp>
#include "Node.h"

using namespace graphski;

TEST_CASE("Node Initialization and Basic Properties", "[Node]") {
    SECTION("Default constructor logic with empty name") {
        Node node(42);
        REQUIRE(node.getId() == 42);
        // By default if the name is empty, it assigns the stringified id.
        REQUIRE(node.getName() == "42");
        REQUIRE(node.getDIn() == 0);
        REQUIRE(node.getDOut() == 0);
        REQUIRE(node.getD() == 0);
        REQUIRE_FALSE(node.isMarked());
    }

    SECTION("Constructor with an explicit name") {
        Node node(10, "SourceNode");
        REQUIRE(node.getId() == 10);
        REQUIRE(node.getName() == "SourceNode");
    }

    SECTION("Setters and modifying degrees") {
        Node node(1);
        node.setName("NewName");
        REQUIRE(node.getName() == "NewName");

        node.setDIn(5);
        node.setDOut(3);
        REQUIRE(node.getDIn() == 5);
        REQUIRE(node.getDOut() == 3);
        REQUIRE(node.getD() == 8);
    }

    SECTION("Marking functionality") {
        Node node(1);
        REQUIRE_FALSE(node.isMarked());
        
        node.mark();
        REQUIRE(node.isMarked());
        
        node.mark(false);
        REQUIRE_FALSE(node.isMarked());
    }

    SECTION("Equality operator") {
        Node n1(10, "A");
        Node n2(10, "B");
        Node n3(20, "A");

        // Equality is defined by ID only in the current Node class implementation
        REQUIRE(n1 == n2);
        REQUIRE_FALSE(n1 == n3);
    }
}
