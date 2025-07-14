#pragma once
#include <vector>
#include <utility>
#include <fstream>
#include <iostream>
#include <limits>
#include "json.hpp"
#include "IGraph.h"

namespace graphski
{
	template<typename NodeT, typename EdgeT> 
	class Graph: public IGraph
	{
	protected:

		// adjacency list contains pairs node : its edges
		using AdjacencyList = std::vector<std::pair<NodeT*, std::vector<EdgeT*>>>;

		AdjacencyList m_adjList;

	public:
		Graph() 
		{
			m_adjList.reserve(INIT_NODES);
		}

		Graph(Graph&) = delete;
		Graph& operator=(Graph&) = delete;
		
		Graph(Graph&& other) 
		{
			m_adjList = std::move(other.m_adjList);
		}
		Graph& operator=(Graph&& other) 
		{
			if (this != &other)
			{
				deleteAdjList();
				m_adjList = std::move(other.m_adjList);
			}
			return *this;
		}
		
		virtual ~Graph() 
		{
			deleteAdjList();
		}

		// clears the graph
		virtual void makeEmpty() override 
		{
			deleteAdjList();
			m_adjList.clear();
			m_adjList.reserve(INIT_NODES);
		};

		// how many nodes are there
		NodeId nodeCount() const override { return (NodeId)m_adjList.size(); }
		// for given node how many edges does it have
		size_t edgeCount(NodeId id) const override
		{
			if(!nodeIdInBounds(id))
				return 0;

			return m_adjList[id].second.size(); 
		}

		// creates a node with empty edges list, returns its unique idf
		virtual NodeId addNode(std::string name = "") override 
		{
			NodeId id = nodeCount(); // TODO: this wont work if nodes can be deleted (ok for now)
			// push back new node with empty edges list
			m_adjList.push_back({ new NodeT(id, name), {} });

			return id;
		}

		// creates an edge between to given nodes, gets them by ids
		virtual void addEdge(NodeId fromNodeId, NodeId toNodeId) override
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
		virtual void markNode(NodeId id, bool val = true) override { getNode(id)->mark(val); }

		// gets an array of neighbor ids for the given node id
		std::vector<NodeId> getNeighbors(NodeId id) const override
		{
			if (!nodeIdInBounds(id))
				return {};

			std::vector<NodeId> result;
			result.reserve(edgeCount(id));

			for (const EdgeT* edge : m_adjList[id].second)
			{
				result.push_back(edge->getTo()->getId());
			}

			return result;
		}
		
		// gets a peek of the adjacency list. all in terms of ids, not pointers
		AdjacencyListPeek getAdjacencyList() const override
		{
			AdjacencyListPeek result;
			result.reserve(m_adjList.size());
			for (const auto& pair : m_adjList)
			{
				std::vector<NodeId> neighbors = getNeighbors(pair.first->getId());
				NodePeek node{ pair.first->getId(), pair.first->getName() };
				result.push_back({ node, neighbors });
			}
			return result;
		}

		// transposes the graph
		virtual void transpose()
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
				NodeId nodeId = pair.first->getId();
				auto& edges = pair.second;

				for (const auto& edge : edges)
				{
					NodeId targetId = edge->getTo()->getId();
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

				std::vector<NodeId> neighbors;
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
		virtual void loadFromFile()
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
				m_adjList.push_back({ deserializeNode(node),
									  {} });

			// add edges
			for (auto& node : j["nodes"])
			{
				NodeId id = node["id"];
				for (NodeId neighbor : node["neighbors"])
					addEdge(id, neighbor);
			}
		};
	
	protected:
		bool nodeIdInBounds(NodeId id) const
		{
			if (id >= nodeCount())
			{
				std::cerr << "Node id out of bounds: " << (int)id << std::endl;
				return false;
			}
			return true;
		}

		// returns the node pointer by id
		NodeT* getNode(NodeId id) const
		{
			if (!nodeIdInBounds(id))
				return nullptr;

			return m_adjList[id].first;
		}

		// returns the edge pointer by its id (pair of node id and edge id inside node's edges vector)
		EdgeT* getEdge(const EdgeId& edgeId) const
		{
			if (!nodeIdInBounds(edgeId.first))
				return nullptr;
			if (!nodeIdInBounds(edgeId.second))
				return nullptr;

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
			return new NodeT(j["id"].get<NodeId>(),
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

		static constexpr NodeId MAX_NODES = std::numeric_limits<NodeId>::max();
		static constexpr NodeId INIT_NODES = 10;
		static constexpr const char* FILE_NAME = "graph.json";
	};

}