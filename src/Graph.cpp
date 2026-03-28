#include "Graph.h"

namespace graphski
{
    Graph::Graph(const Graph &other):
        m_adjList{other.m_adjList},
        m_nodes{} 
    {
        m_nodes.reserve(other.m_nodes.size());

        for (const auto& node : other.m_nodes)
        {
            if(!node) m_nodes.addNode(nullptr); // keep null pointers for deleted nodes
            else 
            {
                auto* ptrToNode = dynamic_cast<Node*>(node.get());
                assert(ptrToNode != nullptr && "non Node* found in other`s NodeStorage");
                m_nodes.addNode(
                    createNode(ptrToNode)
                );
            }
        }   
    }

    OptionalNodeConstRef Graph::getNode(NodeId id) const
    {
        auto* node = m_nodes.getNode(id);
        if (!node)
            return {};
        return std::cref(static_cast<const INode&>(*node));
    }

    NodeId Graph::addNode(std::string_view name)
    {
        NodeId id = m_nodes.addNode(createNode(name));
        if(name.empty()) // set default name to id if not provided
            m_nodes.getNode(id)->setName(std::to_string(id));

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
        if(!m_nodes.deleteNode(nodeId))
            return false; // try to delete from storage

        // clear outgoing edges or pop depending on id
        if(nodeId == m_adjList.size())
            m_adjList.pop_back();
        else m_adjList[nodeId].clear();

        // TODO: do this lazily
        for (auto& neighbors : m_adjList) // clear incoming edges
        {
            auto it = std::remove(neighbors.begin(), neighbors.end(), nodeId);
            if (it != neighbors.end())
                neighbors.erase(it, neighbors.end());
        }

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
        auto* node = getNodeAs<Node>(id);
        if (!node)
            throw std::invalid_argument("Node with given id does not exist.");

        return m_adjList[id]; // return a copy of the neighbors vector
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

    nlohmann::json Graph::serializeNode(NodeId nodeId) const
    {
        const Node* node = getNodeAs<Node>(nodeId);
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