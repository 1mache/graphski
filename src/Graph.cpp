#include "Graph.h"

namespace graphski
{
	Graph::~Graph()
	{
		for(auto pair: m_adjList)
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

	uint8_t Graph::addNode(std::string name)
	{
		uint8_t id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
		// push back new node with empty edges list
		m_adjList.push_back({ new Node(id, name), {} });

		return id;
	}

	void Graph::addEdge(uint8_t fromNodeId, uint8_t toNodeId)
	{
		Node *fromPtr = m_adjList[fromNodeId].first,
			 *toPtr = m_adjList[toNodeId].first;

		Edge* newEdge = new Edge(fromPtr, toPtr);

		// increment degrees
		fromPtr->setDOut(fromPtr->getDOut() + 1);
		toPtr->setDIn(toPtr->getDIn() + 1);

		// put new edge into the vector. TODO: sort by id toId inside vector
		m_adjList[fromNodeId].second.push_back(newEdge);
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
			const Node* node = pair.first;
			std::vector<uint8_t> neighbors;
			for (const Edge* edge : pair.second)
				// get all the id's of the nodes that are connected to this one
				neighbors.push_back(edge->getTo()->getId());

			// write id and neighbors to json
			nodesArr[node->getId()] = {
				{"id", node->getId()},
				{"neighbors", neighbors}
			};
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

		m_adjList.clear();
		m_adjList.reserve(j["nodeCount"].get<uint8_t>());

		// create nodes
		for (auto node : j["nodes"])
		{
			uint8_t id = node["id"];
			m_adjList.push_back({ new Node(id), {} });
		}

		// add edges
		for (auto node : j["nodes"])
		{
			uint8_t id = node["id"];
			for (uint8_t neighbor : node["neighbors"])
				addEdge(id, neighbor);
		}
	}
}