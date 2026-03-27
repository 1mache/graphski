#include "GraphFunctions.h"

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
    swap(first.m_freeNodeIds, second.m_freeNodeIds);
    swap(first.m_nodeCount, second.m_nodeCount);
}

void transposeGraph(Graph &graph) {
    std::cout << "I am transposing it" << std::endl;
    // we can reuse the same nodes vector
    auto& nodes = graph.m_nodes; 
    // new Adjacency list sized like the original and filled with empty vectors
    AdjacencyList newAdjList{ graph.m_adjList.size(), std::vector<NodeId>() };

    // fill it with opposite edges
    for (size_t i = 0; i < graph.m_nodes.size(); ++i)
    {
        NodeId nodeId = static_cast<NodeId>(i);
        if(!graph.getNodeInfo(nodeId).has_value())
            continue; // skip deleted nodes
        
        const auto& neighbors = graph.m_adjList[nodeId];

        for (NodeId neighbor : neighbors)
        {
            if(!graph.getNodeInfo(neighbor).has_value())
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

void saveToFile(const Graph &graph, std::string_view fileName)
{
    // initialize the file
    nlohmann::json j;
    // write the number of nodes
    j["nodeCount"] = graph.nodeCount();
    // write size of m_nodes
    j["nodeMaxId"] = graph.m_nodes.size();
    // initialize the array of neighbors in json 
    auto& nodesArr = j["nodes"] = nlohmann::json::array();

    for (size_t i = 0; i < graph.m_nodes.size(); ++i)
    {
        NodeId nodeId = static_cast<NodeId>(i);
        if(!graph.getNodeInfo(nodeId).has_value())
            continue; // skip deleted nodes
        auto nodeJson = graph.serializeNode(nodeId);

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

    graph.makeEmpty(); // clear the graph before loading
    graph.m_nodeCount = j["nodeCount"].get<NodeId>();
    graph.m_nodes.resize(j["nodeMaxId"].get<NodeId>());
    graph.m_adjList.resize(j["nodeMaxId"].get<NodeId>(), std::vector<NodeId>{});

    // create nodes
    for (auto& nodeJson : j["nodes"])
    {
        // add the node to the graph's nodes vector at the correct index (id)
        graph.m_nodes[nodeJson["id"]] = graph.deserializeNode(nodeJson);
    }

    // add edges
    for (auto& nodeJson : j["nodes"])
    {
        NodeId id = nodeJson["id"];
        for (NodeId neighbor : nodeJson["neighbors"])
            graph.m_adjList[id].push_back(neighbor);
    }
}
} // namespace graphski