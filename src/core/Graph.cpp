#include <algorithm>
#include <ranges>

#include "core/Graph.h"

namespace graphski::core
{
Graph::Graph(const Graph &other):
    m_adjList{other.m_adjList},
    m_nodeStorage{} 
{
    m_nodeStorage.reserve(other.m_nodeStorage.size());

    for (const auto& node : other.m_nodeStorage)
    {
        if(!node) m_nodeStorage.addNode(nullptr); // keep null pointers for deleted nodes
        else 
        {
            auto* ptrToNode = dynamic_cast<Node*>(node.get());
            assert(ptrToNode != nullptr && "non Node* found in other`s NodeStorage");
            m_nodeStorage.addNode(
                createNode(ptrToNode)
            );
        }
    }   
}

size_t Graph::edgeCount() const
{
    auto cleanedAdjList = getAdjacencyList();
    return std::accumulate(cleanedAdjList.begin(), cleanedAdjList.end(), 0u,
        [](size_t sum, const auto& neighbors) { return sum + neighbors.size();});
}

OptionalNodeConstRef Graph::getNode(NodeId id) const
{
    auto* node = m_nodeStorage.getNode(id);
    if (!node)
        return {};
    return std::cref(static_cast<const INode&>(*node));
}

std::vector<NodeId> Graph::getNodeIds() const
{
    std::vector<NodeId> res;
    res.reserve(m_nodeStorage.size());

    NodeId id = 0;
    for (const auto& node : m_nodeStorage)
    {
        if (node) res.push_back(id);

        ++id;
    }

    return res;
}

NodeId Graph::addNode(std::string_view name)
{
    // if we reuse same id that was deleted and it had
    // in edges that we didnt delete the behavior is unexpected
    // need to clean up
    ensureCleanAdjList();

    NodeId id = m_nodeStorage.addNode(createNode(name));
    if(name.empty()) // set default name to id if not provided
        m_nodeStorage.getNode(id)->setName(std::to_string(id));

    if(m_adjList.size() <= id) // add an empty neighbors vector if id is not reused
        m_adjList.emplace_back(); 
    return id;
}

void Graph::addEdge(NodeId fromNodeId, NodeId toNodeId)
{
    Node* fromPtr = getNodeAs<Node>(fromNodeId),
        * toPtr = getNodeAs<Node>(toNodeId);

    if (!fromPtr || !toPtr)
    {
        std::cerr << "Error: trying to add edge between non-existing nodes: "
            << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
        throw std::invalid_argument("Invalid node id(s) provided for edge creation.");
    }

    // check if edge already exists
    if (edgeExists(fromNodeId, toNodeId))
        return; // edge already exists

    // increment degrees
    fromPtr->setDegOut(fromPtr->getDegOut() + 1);
    toPtr->setDegIn(toPtr->getDegIn() + 1);

    m_adjList[fromNodeId].push_back(toNodeId);
}

bool Graph::deleteNode(NodeId nodeId)
{
    bool lastInStorage = (m_nodeStorage.maxNodeId() == nodeId);
    if(!m_nodeStorage.deleteNode(nodeId))
        return false; // try to delete from storage

    // clear outgoing edges or pop depending on id
    if(lastInStorage)
        m_adjList.pop_back();
    else
    {
        m_adjList[nodeId].clear();
        m_adjListNeedsCleanup = true; // lazy cleanup for in edges flag
    } 
    
    // if last was popped there are possible trailing nulls. pop them
    while(m_nodeStorage.size() > m_adjList.size())
        m_adjList.pop_back();

    // incoming edges deletion is handled lazily:
    // 1. when asked for neighbors of node.
    // 2. when asked for adjacency list
    // 3. when asked for total amount of edges.


    return true;
}

bool Graph::deleteEdge(NodeId fromNodeId, NodeId toNodeId)
{
    Node* fromPtr = getNodeAs<Node>(fromNodeId),
        * toPtr = getNodeAs<Node>(toNodeId);

    if (!fromPtr || !toPtr)
    {
        std::cerr << "Error: trying to add edge between non-existing nodes: "
            << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
        throw std::invalid_argument("Invalid node id(s) provided for edge creation.");
    }

    auto& neighbors = m_adjList[fromNodeId];
    if(std::erase(neighbors, toNodeId) > 0) // if it actually erased 
    {
        fromPtr->setDegOut(fromPtr->getDegOut() - 1);
        toPtr->setDegIn(toPtr->getDegIn() - 1);
        return true; // edge was deleted
    }
    
    return false; // edge did not exist
}

std::vector<NodeId> Graph::getNeighborsOf(NodeId id) const
{
    using namespace std::ranges;
    auto* node = getNodeAs<Node>(id);
    if (!node)
        throw std::invalid_argument("Node with given id does not exist.");

    auto neighborsCleaned = m_adjList[id]; 
    // remove edges to deleted nodes
    const auto [first, last] = remove_if(neighborsCleaned, 
                [this](NodeId id){return m_nodeStorage.getNode(id) == nullptr;});
    
    neighborsCleaned.erase(first,last);
        
    return neighborsCleaned; // return a cleaned up copy of the neighbors vector
}

AdjacencyList Graph::getAdjacencyList() const
{
    auto adjListCleaned = m_adjList;
    if(m_adjListNeedsCleanup)
        cleanupIncomingEdgesInList(adjListCleaned);

    return adjListCleaned;
}

bool Graph::edgeExists(NodeId fromNodeId, NodeId toNodeId) const
{
    if (!getNode(fromNodeId) || !getNode(toNodeId))
        return false; // check for both nodes existence

    const auto& neighbors = m_adjList[fromNodeId];
    auto it = std::find_if(neighbors.begin(), neighbors.end(),
        [toNodeId](NodeId neighbor) { return neighbor == toNodeId; });

    if (it == neighbors.end())
        return false;

    return true;
}

void Graph::cleanupIncomingEdgesInList(AdjacencyList& adjList) const
{
    using namespace std::ranges;
    for (auto& neighbors : adjList)
    {
        // remove edges to deleted nodes
        const auto [first, last] = remove_if(neighbors, 
                    [this](NodeId id){return m_nodeStorage.getNode(id) == nullptr;});
        
        neighbors.erase(first,last);
    }
}

void Graph::ensureCleanAdjList()
{
    if(m_adjListNeedsCleanup)
    {
        cleanupIncomingEdgesInList(m_adjList);
        m_adjListNeedsCleanup = false;
    }
}
} // namespace graphski::core