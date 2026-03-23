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
	public:
		enum class Distributions
		{
			Uniform, IndexDependent
		};

	public:
		RandomGraphGenerator(IGraph& targetGraph):
			m_graph(targetGraph)
		{}

		RandomGraphGenerator(const RandomGraphGenerator&) = delete;
		RandomGraphGenerator& operator=(const RandomGraphGenerator&) = delete;

		
		// generate the graph with the given number of nodes and edge probabilities
		// p[i] = probability edge to exist from some node j to node i
		void generate(const std::vector<float>& probabilities = {});
		void generate(Distributions type)
		{
			generate(generateProbabilities(type));
		}

		private:
		// returns vector of probabilities based on the given distribution type
		std::vector<float> generateProbabilities(Distributions type);
		
		
		IGraph& m_graph;
		
		// random number generator
		std::random_device rd;
		std::mt19937 gen{ rd() };
		std::uniform_real_distribution<> distribution{ 0.0, 1.0 };
	};
}