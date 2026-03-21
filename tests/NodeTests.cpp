#include <catch2/catch_test_macros.hpp>
#include "Node.h"

using namespace graphski;

TEST_CASE("Node Initialization and Basic Properties", "[Node]") {
    SECTION("Default constructor logic") {
        Node node("node");
        // By default if the name is empty, it assigns the stringified id.
        REQUIRE(node.getName() == "node");
        REQUIRE(node.getDegIn() == 0);
        REQUIRE(node.getDegOut() == 0);
        REQUIRE(node.getDegree() == 0);
    }

    SECTION("Setters and modifying degrees") {
        Node node("node");
        node.setName("NewName");
        REQUIRE(node.getName() == "NewName");

        node.setDegIn(5);
        node.setDOut(3);
        REQUIRE(node.getDegIn() == 5);
        REQUIRE(node.getDegOut() == 3);
        REQUIRE(node.getDegree() == 8);
    }
}
