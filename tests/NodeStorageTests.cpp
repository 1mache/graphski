#include <catch2/catch_test_macros.hpp>

#include <memory>

#include "Node.h"
#include "NodeStorage.h"

using namespace graphski;

TEST_CASE("NodeStorage starts empty", "[NodeStorage]")
{
    NodeStorage storage;

    REQUIRE(storage.size() == 0);
    REQUIRE(storage.nodeCount() == 0);
    REQUIRE(storage.getNode(0) == nullptr);
}

TEST_CASE("NodeStorage addNode stores and returns sequential ids", "[NodeStorage]")
{
    NodeStorage storage;

    NodeId id0 = storage.addNode(std::make_unique<Node>("A"));
    NodeId id1 = storage.addNode(std::make_unique<Node>("B"));

    REQUIRE(id0 == 0);
    REQUIRE(id1 == 1);
    REQUIRE(storage.size() == 2);
    REQUIRE(storage.nodeCount() == 2);

    auto* node0 = storage.getNode(id0);
    auto* node1 = storage.getNode(id1);
    REQUIRE(node0 != nullptr);
    REQUIRE(node1 != nullptr);
    REQUIRE(node0->getName() == "A");
    REQUIRE(node1->getName() == "B");
}

TEST_CASE("NodeStorage const getter returns stable node view", "[NodeStorage]")
{
    NodeStorage storage;
    NodeId id = storage.addNode(std::make_unique<Node>("ConstNode"));

    const NodeStorage& constStorage = storage;
    const INode* node = constStorage.getNode(id);

    REQUIRE(node != nullptr);
    REQUIRE(node->getName() == "ConstNode");
}

TEST_CASE("NodeStorage deleteNode fails on missing or invalid ids", "[NodeStorage]")
{
    NodeStorage storage;
    NodeId id0 = storage.addNode(std::make_unique<Node>("A"));

    REQUIRE_FALSE(storage.deleteNode(100));
    REQUIRE_FALSE(storage.deleteNode(static_cast<NodeId>(id0 + 1)));

    REQUIRE(storage.size() == 1);
    REQUIRE(storage.nodeCount() == 1);
    REQUIRE(storage.getNode(id0) != nullptr);
}

TEST_CASE("NodeStorage deleteNode on non-last id frees slot and next add reuses id", "[NodeStorage]")
{
    NodeStorage storage;
    NodeId id0 = storage.addNode(std::make_unique<Node>("A"));
    NodeId id1 = storage.addNode(std::make_unique<Node>("B"));
    NodeId id2 = storage.addNode(std::make_unique<Node>("C"));

    REQUIRE(storage.deleteNode(id1));
    REQUIRE(storage.size() == 3);
    REQUIRE(storage.nodeCount() == 2);
    REQUIRE(storage.getNode(id1) == nullptr);

    NodeId reusedId = storage.addNode(std::make_unique<Node>("R"));
    REQUIRE(reusedId == id1);
    REQUIRE(storage.size() == 3);
    REQUIRE(storage.nodeCount() == 3);

    auto* reusedNode = storage.getNode(reusedId);
    REQUIRE(reusedNode != nullptr);
    REQUIRE(reusedNode->getName() == "R");

    REQUIRE(storage.getNode(id0) != nullptr);
    REQUIRE(storage.getNode(id2) != nullptr);
}

TEST_CASE("NodeStorage deleteNode on last id shrinks storage", "[NodeStorage]")
{
    NodeStorage storage;
    NodeId id0 = storage.addNode(std::make_unique<Node>("A"));
    NodeId id1 = storage.addNode(std::make_unique<Node>("B"));

    REQUIRE(storage.deleteNode(id1));
    REQUIRE(storage.size() == 1);
    REQUIRE(storage.nodeCount() == 1);
    REQUIRE(storage.getNode(id1) == nullptr);
    REQUIRE(storage.getNode(id0) != nullptr);

    NodeId newId = storage.addNode(std::make_unique<Node>("C"));
    REQUIRE(newId == 1);
}

TEST_CASE("NodeStorage clear resets internal state and id allocation", "[NodeStorage]")
{
    NodeStorage storage;
    storage.addNode(std::make_unique<Node>("A"));
    NodeId deleted = storage.addNode(std::make_unique<Node>("B"));
    storage.addNode(std::make_unique<Node>("C"));
    REQUIRE(storage.deleteNode(deleted));

    storage.clear();

    REQUIRE(storage.size() == 0);
    REQUIRE(storage.nodeCount() == 0);

    NodeId idAfterClear = storage.addNode(std::make_unique<Node>("AfterClear"));
    REQUIRE(idAfterClear == 0);
    REQUIRE(storage.nodeCount() == 1);
}

TEST_CASE("NodeStorage accepts null node slots without counting them as active nodes", "[NodeStorage]")
{
    NodeStorage storage;

    NodeId nullId = storage.addNode(nullptr);
    REQUIRE(nullId == 0);
    REQUIRE(storage.size() == 1);
    REQUIRE(storage.nodeCount() == 0);
    REQUIRE(storage.getNode(nullId) == nullptr);

    NodeId id1 = storage.addNode(std::make_unique<Node>("Real"));
    REQUIRE(id1 == 1);
    REQUIRE(storage.size() == 2);
    REQUIRE(storage.nodeCount() == 1);
    REQUIRE(storage.getNode(id1) != nullptr);
}