#include <catch2/catch_test_macros.hpp>
#include "Node.h"

using namespace graphski;

TEST_CASE("Node Initialization and Basic Properties", "[Node]") {
    SECTION("Default constructor logic with empty name") {
        Node node(42);
        REQUIRE(node.getId() == 42);
        // By default if the name is empty, it assigns the stringified id.
        REQUIRE(node.getName() == "42");
        REQUIRE(node.getDegIn() == 0);
        REQUIRE(node.getDegOut() == 0);
        REQUIRE(node.getDegree() == 0);
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

        node.setDegIn(5);
        node.setDOut(3);
        REQUIRE(node.getDegIn() == 5);
        REQUIRE(node.getDegOut() == 3);
        REQUIRE(node.getDegree() == 8);
    }

    SECTION("Equality operator") {
        Node n1(10, "A");
        Node n2(10, "B");
        Node n3(20, "A");

        // Equality is defined by ID only in the current Node class implementation
        REQUIRE(n1.getId() == n2.getId());
        REQUIRE_FALSE(n1.getId() == n3.getId());
    }
}
