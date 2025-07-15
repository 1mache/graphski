#pragma once
#include <vector>
#include <stdexcept>
#include <random>
#include <limits>
#include <iostream>

#include "GraphTypes.h"
#include "IGraph.h"

namespace graphski
{
	class RandomGraphGenerator
	{
		IGraph& m_graph;
		NodeId  m_nodeCount = 0;

		// nodes[i] = id of node i
		std::vector<NodeId> m_nodes;
		// p[i] = probability edge to exist from some node j to node i
		std::vector<float>  m_nodeProbabilities;

		// random number generator
		std::random_device rd;
		std::mt19937 gen{ rd() };
		std::uniform_real_distribution<> distribution { 0.0, 1.0 };

	public:
		enum class Distributions
		{
			Uniform, iDependent
		};

	public:
		RandomGraphGenerator(IGraph& targetGraph, size_t nodeCount):
			m_graph(targetGraph)
		{
			m_nodeCount = static_cast<NodeId>(nodeCount);
			constexpr auto maxNodeCount = std::numeric_limits<NodeId>::max();
			if(nodeCount > maxNodeCount)
			{
				std::cerr << "Warning: nodeCount exceeds maximum NodeId value, using max value instead.\n";
				m_nodeCount = maxNodeCount;
			}

			m_nodes.reserve(nodeCount);
			m_nodeProbabilities.reserve(nodeCount);
		}

		RandomGraphGenerator(const RandomGraphGenerator&) = delete;
		RandomGraphGenerator& operator=(const RandomGraphGenerator&) = delete;

		// set the edge probabilities
		void setProbabilities(const std::vector<float>& probabilities)
		{
			if (probabilities.size() != m_nodeCount)
				throw std::invalid_argument("Probabilities size must match the number of nodes.");

			m_nodeProbabilities = probabilities;
		}
		void setProbabilities(Distributions type);

		// generate the graph with the given number of nodes and edge probabilities
		void generate();
	};
}