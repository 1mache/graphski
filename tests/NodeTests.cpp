#include <catch2/catch_test_macros.hpp>
#include "core/Node.h"

using namespace graphski;

TEST_CASE("Node Initialization and Basic Properties", "[Node]") {
    SECTION("Default constructor logic") {
        Node node("node");
        // By default if the name is empty, it assigns the stringified id.
        REQUIRE(node.getName() == "node");
        REQUIRE(node.getDegIn() == 0);
        REQUIRE(node.getDegOut() == 0);
    }

    SECTION("Setters and modifying degrees") {
        Node node("node");
        node.setName("NewName");
        REQUIRE(node.getName() == "NewName");

        node.setDegIn(5);
        node.setDegOut(3);
        REQUIRE(node.getDegIn() == 5);
        REQUIRE(node.getDegOut() == 3);
    }
}
