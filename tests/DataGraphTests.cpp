#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string>
#include <utility>

#include "DataGraph.h"

using namespace graphski;

namespace
{
struct TrackingPayload
{
    static inline int copyCtorCount = 0;
    static inline int moveCtorCount = 0;

    static void resetCounters()
    {
        copyCtorCount = 0;
        moveCtorCount = 0;
    }

    std::string value;

    TrackingPayload() = default;
    explicit TrackingPayload(std::string v) : value(std::move(v)) {}

    TrackingPayload(const TrackingPayload& other) : value(other.value)
    {
        ++copyCtorCount;
    }

    TrackingPayload(TrackingPayload&& other) noexcept : value(std::move(other.value))
    {
        ++moveCtorCount;
    }

    TrackingPayload& operator=(const TrackingPayload&) = default;
    TrackingPayload& operator=(TrackingPayload&&) noexcept = default;
};
} // namespace

TEST_CASE("DataGraph adds nodes and stores payload", "[DataGraph]")
{
    DataGraph<std::string> graph;

    NodeId id0 = graph.addNode("payload0", "A");
    NodeId id1 = graph.addNode(std::string("payload1"), "B");

    REQUIRE(id0 == 0);
    REQUIRE(id1 == 1);
    REQUIRE(graph.nodeCount() == 2);

    REQUIRE(graph.node(id0).getName() == "A");
    REQUIRE(graph.node(id1).getName() == "B");

    REQUIRE(graph.getNodeData(id0) == "payload0");
    REQUIRE(graph.getNodeData(id1) == "payload1");
}

TEST_CASE("DataGraph base-signature addNode keeps data vector aligned", "[DataGraph]")
{
    DataGraph<int> graph;

    NodeId id = graph.addNode("OnlyName");

    REQUIRE(id == 0);
    REQUIRE(graph.nodeCount() == 1);
    REQUIRE(graph.getNodeData(id) == 0);
}

TEST_CASE("DataGraph retrieveNodeData returns data without moving from storage", "[DataGraph]")
{
    DataGraph<std::string> graph;
    NodeId id = graph.addNode(std::string("keep-me"), "N");

    std::string& retrieved = graph.getNodeData(id);

    REQUIRE(retrieved == "keep-me");
    REQUIRE(graph.getNodeData(id) == "keep-me");
}

TEST_CASE("DataGraph throws on out-of-range id", "[DataGraph]")
{
    DataGraph<int> graph;

    REQUIRE_THROWS_AS(graph.getNodeData(0), std::out_of_range);
}

TEST_CASE("DataGraph through Graph reference still keeps data in sync", "[DataGraph][Polymorphism]")
{
    DataGraph<int> graph;
    Graph& baseRef = graph;

    NodeId id = baseRef.addNode("FromBaseRef");

    REQUIRE(graph.nodeCount() == 1);
    REQUIRE(id == 0);
    REQUIRE(graph.getNodeData(id) == 0);
}

TEST_CASE("DataGraph addNode chooses copy for lvalue and move for rvalue", "[DataGraph][Overload]")
{
    SECTION("lvalue insertion uses copy construction")
    {
        TrackingPayload::resetCounters();
        DataGraph<TrackingPayload> graph;

        TrackingPayload payload{"lvalue-data"};
        NodeId id = graph.addNode(payload, "L");

        REQUIRE(id == 0);
        REQUIRE(graph.getNodeData(id).value == "lvalue-data");
        REQUIRE(TrackingPayload::copyCtorCount >= 1);
    }

    SECTION("rvalue insertion uses move construction")
    {
        TrackingPayload::resetCounters();
        DataGraph<TrackingPayload> graph;

        NodeId id = graph.addNode(TrackingPayload{"rvalue-data"}, "R");

        REQUIRE(id == 0);
        REQUIRE(graph.getNodeData(id).value == "rvalue-data");
        REQUIRE(TrackingPayload::moveCtorCount >= 1);
        REQUIRE(TrackingPayload::copyCtorCount == 0);
    }
}

TEST_CASE("DataGraph Rule of Five", "[DataGraph][RuleOf5]")
{
    SECTION("Constructors create valid DataGraph")
    {
        DataGraph<int> gDefault;
        DataGraph<int> gReserved(32);

        REQUIRE(gDefault.nodeCount() == 0);
        REQUIRE(gDefault.edgeCount() == 0);
        REQUIRE(gReserved.nodeCount() == 0);
        REQUIRE(gReserved.edgeCount() == 0);
    }

    SECTION("Copy constructor copies graph topology and payload")
    {
        DataGraph<std::string> source;
        NodeId a = source.addNode("data-A", "A");
        NodeId b = source.addNode("data-B", "B");
        source.addEdge(a, b);

        DataGraph<std::string> copy(source);

        REQUIRE(copy.nodeCount() == source.nodeCount());
        REQUIRE(copy.edgeCount() == source.edgeCount());
        REQUIRE(copy.node(a).getName() == "A");
        REQUIRE(copy.node(b).getName() == "B");
        REQUIRE(copy.getNodeData(a) == "data-A");
        REQUIRE(copy.getNodeData(b) == "data-B");

        source.getNodeData(a) = "changed";
        source.deleteEdge(a, b);

        REQUIRE(copy.getNodeData(a) == "data-A");
        REQUIRE(copy.edgeCount() == 1);
    }

    SECTION("Move constructor transfers graph topology and payload")
    {
        DataGraph<std::string> source;
        NodeId a = source.addNode("data-A", "A");
        NodeId b = source.addNode("data-B", "B");
        source.addEdge(a, b);

        DataGraph<std::string> moved(std::move(source));

        REQUIRE(moved.nodeCount() == 2);
        REQUIRE(moved.edgeCount() == 1);
        REQUIRE(moved.node(a).getName() == "A");
        REQUIRE(moved.node(b).getName() == "B");
        REQUIRE(moved.getNodeData(a) == "data-A");
        REQUIRE(moved.getNodeData(b) == "data-B");

        source.makeEmpty();
        NodeId x = source.addNode("new-data", "X");
        REQUIRE(x == 0);
        REQUIRE(source.nodeCount() == 1);
        REQUIRE(source.getNodeData(x) == "new-data");
    }

    SECTION("Copy assignment copies graph topology and payload")
    {
        DataGraph<std::string> source;
        NodeId a = source.addNode("data-A", "A");
        NodeId b = source.addNode("data-B", "B");
        source.addEdge(a, b);

        DataGraph<std::string> target;
        target.addNode("old-data", "Old");

        target = source;

        REQUIRE(target.nodeCount() == 2);
        REQUIRE(target.edgeCount() == 1);
        REQUIRE(target.node(a).getName() == "A");
        REQUIRE(target.node(b).getName() == "B");
        REQUIRE(target.getNodeData(a) == "data-A");
        REQUIRE(target.getNodeData(b) == "data-B");

        source.getNodeData(a) = "changed";
        REQUIRE(target.getNodeData(a) == "data-A");
    }

    SECTION("Move assignment transfers graph topology and payload")
    {
        DataGraph<std::string> source;
        NodeId a = source.addNode("data-A", "A");
        NodeId b = source.addNode("data-B", "B");
        source.addEdge(a, b);

        DataGraph<std::string> target;
        target.addNode("old-data", "Old");
        target.addNode("old-data-2", "Old2");

        target = std::move(source);

        REQUIRE(target.nodeCount() == 2);
        REQUIRE(target.edgeCount() == 1);
        REQUIRE(target.node(a).getName() == "A");
        REQUIRE(target.node(b).getName() == "B");
        REQUIRE(target.getNodeData(a) == "data-A");
        REQUIRE(target.getNodeData(b) == "data-B");

        source.makeEmpty();
        NodeId x = source.addNode("after-move", "X");
        REQUIRE(x == 0);
        REQUIRE(source.nodeCount() == 1);
        REQUIRE(source.getNodeData(x) == "after-move");
    }
}
