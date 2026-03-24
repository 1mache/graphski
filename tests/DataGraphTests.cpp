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

    std::string retrieved = graph.retrieveNodeData(id);

    REQUIRE(retrieved == "keep-me");
    REQUIRE(graph.getNodeData(id) == "keep-me");
}

TEST_CASE("DataGraph throws on out-of-range id", "[DataGraph]")
{
    DataGraph<int> graph;

    REQUIRE_THROWS_AS(graph.getNodeData(0), std::out_of_range);
    REQUIRE_THROWS_AS(graph.retrieveNodeData(0), std::out_of_range);
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
