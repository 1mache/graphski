#include "GraphFunctions.h"
namespace graphski
{
void transposeGraph(Graph &graph)
{
    std::cout << "I am transposing it" << std::endl;
    // we can reuse the same nodes vector
    auto& nodes = graph.m_nodes; 
    // new Adjacency list sized like the original and filled with empty vectors
    AdjacencyList newAdjList{ graph.m_adjList.size(), std::vector<NodeId>() };

    // fill it with opposite edges
    for (size_t i = 0; i < graph.nodeCount(); ++i)
    {
        NodeId nodeId = static_cast<NodeId>(i);
        const auto& neighbors = graph.m_adjList[nodeId];

        for (NodeId neighbor : neighbors)
            newAdjList[neighbor].push_back(nodeId);
    }

    std::for_each(nodes.begin(), nodes.end(), 
        [](Node* node) {
            // swap in and out degrees for each node
            NodeId dIn = node->getDIn();
            node->setDIn(node->getDOut());
            node->setDOut(dIn);
        });
    
    graph.m_adjList = std::move(newAdjList);
}
} // namespace graphski