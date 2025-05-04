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
		constexpr int INDENT_SIZE = 2;
		int indentCount = 0;

		std::ofstream file(FILE_NAME);
		if (!file.is_open())
		{
			std::cout << "Error! Failed to create file!" << std::endl;
			return;
		}

		file << "{\n";
		indentCount ++;
		file << std::string(indentCount * INDENT_SIZE, ' ');
		file << "\"nodeCount\": " << m_adjList.size() << ",\n";
		file << std::string(indentCount * INDENT_SIZE, ' ');
		file << "\"nodes\": [\n";
		indentCount ++;

		for (uint8_t i = 0; i < m_adjList.size(); i++)
		{
			file << std::string(indentCount * INDENT_SIZE, ' ');
			file << "{\n";
			indentCount++;
			
			file << std::string(indentCount * INDENT_SIZE, ' ');
			file << "\"id\": " << (int)m_adjList[i].first->getId() << ",\n";
			
			file << std::string(indentCount * INDENT_SIZE, ' ');
			file << "\"neighbors\": [";

			for (uint8_t j = 0; j < m_adjList[i].second.size(); j++)
			{
				file << (int)m_adjList[i].second[j]->getTo()->getId();
				// add coma if not last
				if (j != m_adjList[i].second.size() - 1)
					file << ", ";
			}
			file << "]\n";

			indentCount--;
			file << std::string(indentCount * INDENT_SIZE, ' ');
			file << "}";
			// add coma if not last
			if (i != m_adjList.size() - 1)
				file << ", ";
			file << "\n";
		}

		indentCount--;
		file << std::string(indentCount * INDENT_SIZE, ' ');
		file << "]\n";

		indentCount--;
		file << std::string(indentCount * INDENT_SIZE, ' ');
		file << "}\n";

		std::cout << "Successfully wrote to: " << FILE_NAME << std::endl;
		file.close();
	}

	//{
	//	nlohmann::json j;
	//	auto& nodesArr = j["nodes"] = nlohmann::json::array();
	//	j["nodeCount"] = nodeCount();

	//	for (const auto& pair : m_adjList)
	//	{
	//		const Node* node = pair.first;
	//		std::vector<uint8_t> neighbors;
	//		for (const Edge* edge : pair.second)
	//		{
	//			// get the id of the nodes that are connected to this one
	//			neighbors.push_back(edge->getTo()->getId());
	//		}

	//		nodesArr[node->getId()] = {
	//			{"id", node->getId()},
	//			{"name", node->getName()},
	//			{"neighbors", neighbors}
	//		};
	//	}

	//	std::ofstream file(FILE_NAME);
	//	if (file.is_open())
	//	{
	//		file << j.dump(2); // pretty print with 2 spaces
	//		file.close();
	//		std::cout << "Graph saved to " << FILE_NAME << std::endl;
	//	}
	//	else
	//	{
	//		std::cout << "Error opening file for writing: " << FILE_NAME << std::endl;
	//	}
	//}
}