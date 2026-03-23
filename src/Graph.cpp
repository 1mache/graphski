#include "Graph.h"

namespace graphski
{
    Graph::Graph(size_t reserveCount)
    {
        if (reserveCount > MAX_NODES)
        {
            std::cout << "Too much nodes requested, max is " << MAX_NODES << '\n';
            reserveCount = RESERVE_NODES;
        }
        else if (reserveCount == 0)
            reserveCount = RESERVE_NODES;

        m_nodes.reserve(reserveCount);
        m_adjList.reserve(reserveCount);
    }

    Graph::Graph(const Graph& other)
        :m_nodes{},
         m_adjList{other.m_adjList} // copy adj list.
    {
        m_nodes.reserve(other.nodeCount());

        for (const auto& pnode : other.m_nodes)
            m_nodes.push_back(createNode(pnode.get()));
    }

    void Graph::makeEmpty()
    {
        m_nodes.clear();
        m_adjList.clear();
        m_nodes.reserve(RESERVE_NODES);
        m_adjList.reserve(RESERVE_NODES);    
    }

    const Node& Graph::node(NodeId id) const
    {
        auto* node = getNode(id);
        return *node;
    }

    NodeId Graph::addNode(std::string_view name)
    {
        if (nodeCount() >= MAX_NODES)
            throw std::overflow_error("Maximum number of nodes exceeded");

        // TODO: this wont work if nodes can be deleted (ok for now)
        NodeId id = static_cast<NodeId>(nodeCount());
        // default to id as name
        std::string finalName = name.empty() ? std::to_string(id) : std::string(name);

        m_nodes.push_back(createNode(finalName)); // create the node and add it to the graph
        m_adjList.emplace_back(); // add an empty neighbors vector
        return id;
    }

    void Graph::addEdge(NodeId fromNodeId, NodeId toNodeId)
    {
        Node* fromPtr = getNode(fromNodeId),
            * toPtr = getNode(toNodeId);

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

    bool Graph::deleteEdge(NodeId fromNodeId, NodeId toNodeId)
    {
        Node* fromPtr = getNode(fromNodeId),
            * toPtr = getNode(toNodeId);

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

    std::vector<NodeId> Graph::getNeighbors(NodeId id) const
    {
        auto* node = getNode(id);
        if (!node)
            throw std::invalid_argument("Node with given id does not exist.");

        return m_adjList[id]; // return a copy of the neighbors vector
    }

    bool Graph::nodeIdInBounds(NodeId id) const
    {
        if (id >= nodeCount())
        {
            std::cerr << "Node id out of bounds: " << (int)id << std::endl;
            return false;
        }
        return true;
    }

    bool Graph::edgeExists(NodeId fromNodeId, NodeId toNodeId) const
    {
        const auto& neighbors = m_adjList[fromNodeId];
        auto it = std::find_if(neighbors.begin(), neighbors.end(),
            [toNodeId](NodeId neighbor) { return neighbor == toNodeId; });

        if (it != neighbors.end())
            return true;
        return false;
    }

    Node* Graph::getNode(NodeId id)
    {
        if (!nodeIdInBounds(id))
            return nullptr;
        return m_nodes[id].get();
    }

    const Node* Graph::getNode(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return nullptr;
        return m_nodes[id].get();
    }

    nlohmann::json Graph::serializeNode(NodeId nodeId) const
    {
        const Node* node = getNode(nodeId);
        nlohmann::json j;
        j["id"] = nodeId;
        j["name"] = node->getName();
        return j;
    }

    std::unique_ptr<Node> Graph::deserializeNode(const nlohmann::json& j) const
    {
        return createNode(j["name"].get<std::string>());
    }

} // namespace graphski