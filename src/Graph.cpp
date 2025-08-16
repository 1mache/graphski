#include "Graph.h"

namespace graphski
{
    Graph::Graph(size_t reserveCount)
    {
        if (reserveCount > size_t(MAX_NODES))
        {
            std::cout << "Too much nodes requested, max is " << MAX_NODES << '\n';
            reserveCount = size_t(INIT_NODES);
        }
        else if (reserveCount == 0)
            reserveCount = size_t(INIT_NODES);

        m_adjList.reserve(reserveCount);
    }

    Graph::Graph(Graph&& other)
    {
        deleteAdjList();
        m_adjList = std::move(other.m_adjList);
    }

    Graph& Graph::operator=(Graph&& other)
    {
        if (this != &other)
        {
            deleteAdjList();
            m_adjList = std::move(other.m_adjList);
        }
        return *this;
    }

    void Graph::makeEmpty()
    {
        deleteAdjList();
        m_adjList.clear();
        m_adjList.reserve(INIT_NODES);
    }

    size_t Graph::edgeCount() const
    {
        size_t count = 0;
        
       std::for_each(m_adjList.begin(), m_adjList.end(),
		   [&](const auto& pair) { count += pair.second.size(); });
        
        return count;
    }

    NodePeek Graph::peekNode(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return { 0, "", 0, 0 };

        const Node* node = m_adjList[id].first;
        return NodePeek(*node);
    }

    NodeId Graph::addNode(std::string name)
    {
        NodeId id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)

        if (id >= MAX_NODES)
            throw std::overflow_error("Maximum number of nodes exceeded");

        m_adjList.push_back({ createNode(id, name), {} });
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

        Edge* newEdge = createEdge(fromPtr, toPtr);

        // increment degrees
        fromPtr->setDOut(fromPtr->getDOut() + 1);
        toPtr->setDIn(toPtr->getDIn() + 1);

        m_adjList[fromNodeId].second.push_back(newEdge);
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

        auto& edges = m_adjList[fromNodeId].second;
        auto it = std::find_if(edges.begin(), edges.end(),
            [&](Edge* edge) { return edge->getTo()->getId() == toNodeId; });

        if (it == edges.end())
            return false; // edge does not exist, nothing to delete

        edges.erase(it);
        fromPtr->setDOut(fromPtr->getDOut() - 1);
        toPtr->setDIn(toPtr->getDIn() - 1);

        return true; // edge was successfully deleted
    }

    void Graph::markNode(NodeId id, bool val)
    {
        auto* node = getNode(id);
        if (node)
            node->mark(val);
    }

    std::vector<NodeId> Graph::getNeighbors(NodeId id) const
    {
        if (!nodeIdInBounds(id))
            return {};

        std::vector<NodeId> result;

        auto* node = getNode(id);
        if (!node)
            throw std::invalid_argument("Node with given id does not exist.");

        result.reserve(node->getDOut());

        for (const Edge* edge : m_adjList[id].second)
        {
            result.push_back(edge->getTo()->getId());
        }

        return result;
    }

    AdjacencyListPeek Graph::getAdjacencyList() const
    {
        AdjacencyListPeek result;
        result.reserve(m_adjList.size());
        for (const auto& pair : m_adjList)
        {
            Node* node = pair.first;
            result.push_back({ NodePeek(*node), getNeighbors(node->getId()) });
        }
        return result;
    }

    void Graph::transpose()
    {
        std::cout << "I am transposing it" << std::endl;

        // new Adjacency list sized like the original and filled with empty pairs
        AdjacencyList newAdjList{ m_adjList.size(), {nullptr, {}} };

        // fill it with nodes
        for (const auto& pair : m_adjList)
        {
            Node* node = pair.first;

            Node* newNode = createNode(node);
            // swap in and out degrees for new node
            newNode->setDIn(node->getDOut());
            newNode->setDOut(node->getDIn());

            newAdjList[newNode->getId()] = { newNode, {} };
        }

        // fill it with opposite edges
        for (const auto& pair : m_adjList)
        {
            // pointer to new node with same id from new list
            NodeId nodeId = pair.first->getId();
            auto* node = newAdjList[nodeId].first;
            auto& edges = pair.second;

            for (const auto& edge : edges)
            {
                NodeId toId = edge->getTo()->getId();
                // its crucial to use newAdjList here, not m_adjList when accessing nodes
                auto* edgeTarget = newAdjList[toId].first;
                newAdjList[toId].second.push_back(
                    createEdge(edgeTarget, node) // create flipped edge
                );
            }
        }

        // free previous adjlist
        deleteAdjList();
        m_adjList = std::move(newAdjList);
    }

    void Graph::saveToFile() const
    {
        // initialize the file
        nlohmann::json j;
        // write the number of nodes
        j["nodeCount"] = nodeCount();
        // initialize the array of neighbors in json 
        auto& nodesArr = j["nodes"] = nlohmann::json::array();

        for (const auto& pair : m_adjList)
        {
            auto nodeJson = serializeNode(pair.first->getId());

            std::vector<NodeId> neighbors;
            for (const Edge* edge : pair.second)
                // get all the id's of the nodes that are connected to this one
                neighbors.push_back(edge->getTo()->getId());

            nodeJson["neighbors"] = neighbors;
            nodesArr.push_back(nodeJson);
        }

        std::ofstream file(FILE_NAME);
        if (!file.is_open())
        {
            std::cout << "Error opening file for writing: " << FILE_NAME << std::endl;
        }

        file << j.dump(2); // pretty print with 2 spaces (you can change this)
        file.close();
        std::cout << "Graph saved to " << FILE_NAME << std::endl;
    }

    void Graph::loadFromFile()
    {
        nlohmann::json j;
        std::ifstream file(FILE_NAME);
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

        makeEmpty(); // clear the graph before loading
        m_adjList.reserve(j["nodeCount"].get<NodeId>());

        // create nodes
        for (auto& node : j["nodes"])
            m_adjList.push_back({ deserializeNode(node), {} });

        // add edges
        for (auto& node : j["nodes"])
        {
            NodeId id = node["id"];
            for (NodeId neighbor : node["neighbors"])
                addEdge(id, neighbor);
        }
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
        auto& edges = m_adjList[fromNodeId].second;
        auto it = std::find_if(edges.begin(), edges.end(),
            [&](Edge* edge) { return edge->getTo()->getId() == toNodeId; });

        if (it != edges.end())
            return true;
        return false;
    }

    Node* Graph::getNode(NodeId id)
    {
        if (!nodeIdInBounds(id))
            return nullptr;
        return m_adjList[id].first;
    }

    const Node* Graph::getNode(NodeId id) const
    {
        return const_cast<Graph*>(this)->getNode(id);
    }

    Edge* Graph::getEdge(const EdgeLocator& edgeId)
    {
        if (!nodeIdInBounds(edgeId.nodeId))
            return nullptr;

        auto& edges = m_adjList[edgeId.nodeId].second;
        return edges[edgeId.neighborId]; // return the edge at the index
    }

    const Edge* Graph::getEdge(const EdgeLocator& edgeId) const
    {
        return const_cast<Graph*>(this)->getEdge(edgeId);
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
        for (auto pair : m_adjList)
        {
            auto* node = pair.first;

            // delete all edges of the node
            for (auto* edge : pair.second)
            {
                delete edge;
            }
            // delete the node
            delete node;
        }
    }

}