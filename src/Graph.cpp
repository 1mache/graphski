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

    Graph::Graph(Graph&& other)
    {
        deleteAdjList();
        m_nodes = std::move(other.m_nodes);
        m_adjList = std::move(other.m_adjList);
    }

    Graph& Graph::operator=(Graph&& other)
    {
        if (this != &other)
        {
            deleteAdjList();
            m_nodes = std::move(other.m_nodes);
            m_adjList = std::move(other.m_adjList);
        }
        return *this;
    }

    void Graph::makeEmpty()
    {
        deleteAdjList();
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

    NodeId Graph::addNode(std::string name)
    {
        if (nodeCount() >= MAX_NODES)
            throw std::overflow_error("Maximum number of nodes exceeded");

        // TODO: this wont work if nodes can be deleted (ok for now)
        NodeId id = static_cast<NodeId>(nodeCount());

        m_nodes.push_back(createNode(id, name));
        m_adjList.push_back(std::vector<NodeId>());
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
        fromPtr->setDOut(fromPtr->getDOut() + 1);
        toPtr->setDIn(toPtr->getDIn() + 1);

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
            fromPtr->setDOut(fromPtr->getDOut() - 1);
            toPtr->setDIn(toPtr->getDIn() - 1);
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

    // void Graph::saveToFile() const
    // {
    //     // initialize the file
    //     nlohmann::json j;
    //     // write the number of nodes
    //     j["nodeCount"] = nodeCount();
    //     // initialize the array of neighbors in json 
    //     auto& nodesArr = j["nodes"] = nlohmann::json::array();

    //     for (const auto& pair : m_adjList)
    //     {
    //         auto nodeJson = serializeNode(pair.first->getId());

    //         std::vector<NodeId> neighbors;
    //         for (const Edge* edge : pair.second)
    //             // get all the id's of the nodes that are connected to this one
    //             neighbors.push_back(edge->getTo()->getId());

    //         nodeJson["neighbors"] = neighbors;
    //         nodesArr.push_back(nodeJson);
    //     }

    //     std::ofstream file(FILE_NAME);
    //     if (!file.is_open())
    //     {
    //         std::cout << "Error opening file for writing: " << FILE_NAME << std::endl;
    //     }

    //     file << j.dump(2); // pretty print with 2 spaces (you can change this)
    //     file.close();
    //     std::cout << "Graph saved to " << FILE_NAME << std::endl;
    // }

    // void Graph::loadFromFile()
    // {
    //     nlohmann::json j;
    //     std::ifstream file(FILE_NAME);
    //     if (file.is_open())
    //     {
    //         file >> j;
    //         file.close();
    //     }
    //     else
    //     {
    //         std::cout << "Error opening file" << std::endl;
    //         return;
    //     }

    //     makeEmpty(); // clear the graph before loading
    //     m_adjList.reserve(j["nodeCount"].get<NodeId>());

    //     // create nodes
    //     for (auto& node : j["nodes"])
    //         m_adjList.push_back({ deserializeNode(node), {} });

    //     // add edges
    //     for (auto& node : j["nodes"])
    //     {
    //         NodeId id = node["id"];
    //         for (NodeId neighbor : node["neighbors"])
    //             addEdge(id, neighbor);
    //     }
    // }

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
        return m_nodes[id];
    }

    const Node* Graph::getNode(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return nullptr;
        return m_nodes[id];
    }

    nlohmann::json Graph::serializeNode(NodeId nodeId) const
    {
        const Node* node = getNode(nodeId);
        nlohmann::json j;
        j["id"] = node->getId();
        j["name"] = node->getName();
        return j;
    }

    Node* Graph::deserializeNode(const nlohmann::json& j) const
    {
        return createNode(j["id"].get<NodeId>(),
            j["name"].get<std::string>());
    }

    void Graph::deleteAdjList()
    {
        m_adjList.clear();
        for (const auto* node : m_nodes)
        {
            delete node;
        }
        m_nodes.clear();
    }

} // namespace graphski