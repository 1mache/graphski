#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include "json.hpp"

namespace graphski
{
	template<typename NodeT, typename EdgeT> 
	class Graph
	{
	protected:
		// TODO: maybe have edges get ids of the nodes they need instead of pointers, then it could be on the stack
		// EDIT: pointers are for polimorphism, originally. gonna be hard to change now
		
		// adjacency list contains pairs node : its edges
		using AdjacencyList = std::vector<std::pair<NodeT*, std::vector<EdgeT*>>>;
		AdjacencyList m_adjList;

		// pair of node id and edges id inside node's edges vector
		using EdgeId = std::pair<uint8_t, uint8_t>;

	public:
		Graph() 
		{
			m_adjList.reserve(INIT_NODES);
		}

		Graph(Graph&) = delete;
		Graph& operator=(Graph&) = delete;
		virtual ~Graph() 
		{
			deleteAdjList();
		}

		// clears the graph
		virtual void makeEmpty() 
		{
			deleteAdjList();
			m_adjList.clear();
			m_adjList.reserve(INIT_NODES);
		};

		// how many nodes are there
		uint8_t nodeCount() const { return (uint8_t)m_adjList.size(); }
		// for given node how many edges does it have
		uint8_t edgeCount(uint8_t nodeId) const 
		{
			if(nodeId >= nodeCount())
			{
				std::cerr << "Node id out of bounds: " << (int)nodeId << std::endl;
				return 0;
			}

			return (uint8_t)m_adjList[nodeId].second.size(); 
		}

		// returns the copy of the adjacency list (TODO: find better alternative to "peek in")
		AdjacencyList getAdjListCopy() const { return m_adjList;}

		// creates a node with empty edges list, returns its unique idf
		virtual uint8_t addNode(std::string name = "") 
		{
			uint8_t id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
			// push back new node with empty edges list
			m_adjList.push_back({ new NodeT(id, name), {} });

			return id;
		}

		// creates an edge between to given nodes, gets them by ids
		void addEdge(uint8_t fromNodeId, uint8_t toNodeId)
		{
			NodeT *fromPtr = getNode(fromNodeId),
				  *toPtr = getNode(toNodeId);
			
			if(!fromPtr || !toPtr)
			{
				std::cerr << "Error: trying to add edge between non-existing nodes: "
						  << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
				return;
			}

			// check if edge already exists
			for(const EdgeT* edge : m_adjList[fromNodeId].second)
			{
				if (edge->getTo()->getId() == toNodeId)
				{
					std::cerr << "Error: trying to add an edge that already exists between nodes: "
							  << (int)fromNodeId << " and " << (int)toNodeId << std::endl;
					return;
				}
			}	

			EdgeT* newEdge = new EdgeT(fromPtr, toPtr);
			
			// increment degrees
			fromPtr->setDOut(fromPtr->getDOut() + 1);
			toPtr->setDIn(toPtr->getDIn() + 1);

			// put new edge into the vector. TODO: sort by id toId inside vector
			m_adjList[fromNodeId].second.push_back(newEdge);
		}
		
		// marks the node of given id
		void markNode(uint8_t id, bool val = true) { getNode(id)->mark(val); }

		// transposes the graph
		void transpose()
		{
			std::cout << "I am transposing it" << std::endl;

			// new Adjacency list sized like the original and filled with empty pairs
			AdjacencyList newAdjList{ m_adjList.size(), {nullptr, {}} };

			// fill it with nodes
			for (const auto& pair : m_adjList)
			{
				NodeT* node = pair.first;

				NodeT* newNode = new NodeT(*node);
				// swap in and out degrees for new node
				newNode->setDIn(node->getDOut());
				newNode->setDOut(node->getDIn());

				newAdjList[newNode->getId()] = { newNode, {} };
			}

			// fill it with opposite edges
			for (const auto& pair : m_adjList)
			{
				// pointer to new node with same id from new list
				auto nodeId = pair.first->getId();
				auto& edges = pair.second;

				for (const auto& edge : edges)
				{
					uint8_t targetId = edge->getTo()->getId();
					// its crucial to use newAdjList here, not m_adjList when accessing nodes
					auto* edgeTarget = newAdjList[targetId].first;
					newAdjList[targetId].second.push_back(
						new EdgeT(edgeTarget, newAdjList[nodeId].first)
					);
				}
			}

			// free previous adjlist
			deleteAdjList();
			m_adjList = std::move(newAdjList);
		};
	
		// saves the graph to a file in json format
		void saveToFile() const 
		{
			// initialize the file
			nlohmann::json j;
			// write the number of nodes
			j["nodeCount"] = nodeCount();
			// initialize the array of neighbors in json 
			auto& nodesArr = j["nodes"] = nlohmann::json::array();

			for (const auto& pair : m_adjList)
			{
				auto nodeJson = serializeNode(pair.first);

				std::vector<uint8_t> neighbors;
				for (const EdgeT* edge : pair.second)
					// get all the id's of the nodes that are connected to this one
					neighbors.push_back(static_cast<const NodeT*>(edge->getTo())->getId());
				
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
		};

		// loads the graph from a file in json format
		void loadFromFile()
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
			m_adjList.reserve(j["nodeCount"].get<uint8_t>());

			// create nodes
			for (auto& node : j["nodes"])
				m_adjList.push_back({ deserializeNode(node),
									  {} });

			// add edges
			for (auto& node : j["nodes"])
			{
				uint8_t id = node["id"];
				for (uint8_t neighbor : node["neighbors"])
					addEdge(id, neighbor);
			}
		};
	
	protected:
		// returns the node pointer by id
		NodeT* getNode(uint8_t id) const
		{
			if (id >= nodeCount())
			{
				std::cerr << "Node id out of bounds: " << (int)id << std::endl;
				return nullptr;
			}
			return m_adjList[id].first;
		}

		// returns the edge pointer by its id (pair of node id and edge id inside node's edges vector)
		EdgeT* getEdge(const EdgeId& edgeId) const
		{
			if (edgeId.first >= nodeCount())
			{
				std::cerr << "Node id out of bounds: " << (int)edgeId.first << std::endl;
				return nullptr;
			}
			if (edgeId.second >= edgeCount(edgeId.first))
			{
				std::cerr << "Edge id out of bounds: " << (int)edgeId.second << std::endl;
				return nullptr;
			}

			return m_adjList[edgeId.first].second[edgeId.second];
		}

		// retuerns the json representation of the node, used in saveToFie
		virtual nlohmann::json serializeNode(const NodeT* node) const
		{
			nlohmann::json j;
			j["id"] = node->getId();
			j["name"] = node->getName();
			return j;
		}

		// creates and returns a new node given json representation of it, used in loadFromFile
		virtual NodeT* deserializeNode(const nlohmann::json& j) const
		{
			return new NodeT(j["id"].get<uint8_t>(), 
							 j["name"].get<std::string>());
		}

	private:
		void deleteAdjList() 
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
		};

	private: // constants

		static constexpr uint8_t MAX_NODES = UINT8_MAX;
		static constexpr uint8_t INIT_NODES = 10;
		static constexpr const char* FILE_NAME = "graph.json";
	};

}