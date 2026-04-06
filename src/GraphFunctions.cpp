#include "GraphFunctions.h"

#include <unordered_map>
#include "nlohmann/json.hpp"

namespace graphski
{
std::unique_ptr<Node> createNode(std::string_view name)
{
    return std::make_unique<Node>(name);
}

void swap(Graph &first, Graph &second) noexcept
{
    using std::swap;
    swap(first.m_nodes, second.m_nodes);
    swap(first.m_adjList, second.m_adjList);
}

void transposeGraph(Graph &graph) 
{
    std::cout << "I am transposing it" << std::endl;
    // we can reuse the same nodes vector
    auto& nodes = graph.m_nodes; 
    // new Adjacency list sized like the original and filled with empty vectors
    AdjacencyList newAdjList{ graph.m_adjList.size(), std::vector<NodeId>() };

    // fill it with opposite edges
    for (size_t i = 0; i < graph.m_nodes.size(); ++i)
    {
        NodeId nodeId = static_cast<NodeId>(i);
        if(!graph.getNode(nodeId).has_value())
            continue; // skip deleted nodes
        
        const auto& neighbors = graph.m_adjList[nodeId];

        for (NodeId neighbor : neighbors)
        {
            if(!graph.getNode(neighbor).has_value())
                continue; // skip deleted neighbors
            newAdjList[neighbor].push_back(nodeId);
        }
    }

    std::for_each(nodes.begin(), nodes.end(), 
        [](auto& nodep) {
            // swap in and out degrees for each node
            NodeId dIn = nodep->getDegIn();
            nodep->setDegIn(nodep->getDegOut());
            nodep->setDegOut(dIn);
        });
    
    graph.m_adjList = std::move(newAdjList);
}

nlohmann::json serializeINode(const INode &node, NodeId nodeId)
{
    nlohmann::json j;
    j["id"] = nodeId;
    j["name"] = node.getName();

    return j;
}

void saveToFile(const IGraph& graph, std::string_view fileName, const NodeSerializer& nodeSerializer)
{
    // initialize the file
    nlohmann::json j;
    // write the number of nodes
    j["nodeCount"] = graph.nodeCount();
    // initialize the array of neighbors in json 
    auto& nodesArr = j["nodes"] = nlohmann::json::array();

    auto nodeIds = graph.getNodeIds();
    for (NodeId nodeId : nodeIds)
    {
        if(!graph.getNode(nodeId).has_value())
            continue; // skip deleted nodes

        const INode& node = graph.getNode(nodeId).value().get();
        nlohmann::json nodeJson; 

        if (nodeSerializer)
            nodeJson = nodeSerializer(node, nodeId);
        else
            nodeJson = serializeINode(node, nodeId);

        nodeJson["neighbors"] = graph.getNeighborsOf(nodeId); // add neighbors to the node json
        nodesArr.push_back(nodeJson);
    }

    std::ofstream file(fileName.data());
    if (!file.is_open())
    {
        std::cout << "Error opening file for writing: " << fileName << std::endl;
    }

    file << j.dump(2); // pretty print with 2 spaces (you can change this)
    file.close();
    std::cout << "Graph saved to " << fileName << std::endl;
}

void loadFromFile(Graph &graph, std::string_view fileName)
{
    nlohmann::json j;
    std::ifstream file(fileName.data());
    if (file.is_open())
    {
        file >> j;
        file.close();
    }
    else
    {
        std::cout << "Error opening file" << std::endl;
        return;
    }

    graph.clear(); // clear the graph before loading

    // map of old ids to new ids
    std::unordered_map<NodeId, NodeId> old2NewId{j["nodes"].size()};
    // create nodes
    for (auto& nodeJson : j["nodes"])
    {
        NodeId oldId = static_cast<NodeId>(nodeJson["id"]);
        auto newId = graph.addNode(nodeJson["name"]);
        old2NewId[oldId] = newId;
    }

    // add edges
    for (auto& nodeJson : j["nodes"])
    {
        NodeId oldId = nodeJson["id"];
        for (NodeId neighbor : nodeJson["neighbors"])
        {
            // add edges between nodes after translating their ids
            graph.addEdge(old2NewId[oldId], old2NewId[neighbor]);
        }
    }
}
} // namespace graphski