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

    Graph::Graph(const Graph& other):
        m_adjList{other.m_adjList}, // copy adj list.
        m_nodes{}, // empty for now, will populate in the body
        m_freeNodeIds{other.m_freeNodeIds},
        m_nodeCount{other.m_nodeCount}
    {
        m_nodes.reserve(other.m_nodes.size());

        for (const auto& pnode : other.m_nodes)
        {
            if(!pnode) m_nodes.push_back(nullptr); // keep null pointers for deleted nodes
            else m_nodes.push_back(createNode(pnode.get()));
        }
    }

    void Graph::makeEmpty()
    {
        m_nodes.clear();
        m_adjList.clear();
        m_freeNodeIds = std::queue<NodeId>(); // clear the free ids queue
        m_nodeCount = 0;
    }

    OptionalNodeConstRef Graph::node(NodeId id) const
    {
        auto* node = getNode(id);
        if (!node)
            return {};
        return std::cref(static_cast<const INode&>(*node));
    }

    NodeId Graph::addNode(std::string_view name)
    {
        if (nodeCount() >= MAX_NODES)
            throw std::overflow_error("Maximum number of nodes exceeded");

        NodeId id = static_cast<NodeId>(nodeCount()); // default to next id
        // if there is a free id reuse it.
        if (!m_freeNodeIds.empty())
        {
            id = m_freeNodeIds.front();
            m_freeNodeIds.pop();
        }

        // default to id as name
        std::string finalName = name.empty() ? std::to_string(id) : std::string(name);
        auto nodep = createNode(finalName);
        if(m_nodes.size() <= id)
        {
            m_nodes.push_back(std::move(nodep)); // create the node and add it to the graph
            m_adjList.emplace_back(); // add an empty neighbors vector
        }
        else
        {
            m_nodes[id] = std::move(nodep); // create the node and add it to the graph
            m_adjList[id].clear(); // clear any existing neighbors (should be empty already)
        }
        m_nodeCount++;
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

    bool Graph::deleteNode(NodeId nodeId)
    {
        if(!getNode(nodeId))
            return false;

        m_adjList[nodeId].clear(); // clear outgoing edges
        m_nodes[nodeId].reset(nullptr); // delete the node

        // TODO: could this be done lazily?
        for (auto& neighbors : m_adjList) // clear incoming edges
        {
            auto it = std::remove(neighbors.begin(), neighbors.end(), nodeId);
            if (it != neighbors.end())
                neighbors.erase(it, neighbors.end());
        }

        if(nodeId == m_nodes.size() - 1) // if it's the last node, we can just pop it
        {
            m_nodes.pop_back();
            m_adjList.pop_back();
        }    
        else
            m_freeNodeIds.push(nodeId); // add this id to the free list for reuse
            
        m_nodeCount--;
        return true;
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
        if (id >= m_nodes.size())
        {
            std::cerr << "Node id out of bounds: " << (int)id << std::endl;
            return false;
        }
        return true;
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
        if(node)
        {
            j["id"] = nodeId;
            j["name"] = node->getName();
        }
        return j;
    }

    std::unique_ptr<Node> Graph::deserializeNode(const nlohmann::json& j) const
    {
        // TODO: needs to be placed in the correct id.
        return createNode(j["name"].get<std::string>());
    }

} // namespace graphski