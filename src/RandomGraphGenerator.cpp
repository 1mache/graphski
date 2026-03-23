#include "RandomGraphGenerator.h"

namespace graphski
{
	std::vector<float> RandomGraphGenerator::generateProbabilities(Distributions type)
	{
		std::vector<float> nodeProbabilities;
		nodeProbabilities.reserve(m_graph.nodeCount());
		
		if (type == Distributions::Uniform)
			nodeProbabilities.assign(m_graph.nodeCount(), 
									 1.0f / static_cast<float>(m_graph.nodeCount()));

		else if (type == Distributions::IndexDependent)
		{
			for (NodeId i = 0; i < m_graph.nodeCount(); ++i)
				nodeProbabilities.push_back(1.0f / (i + 1));
		}

		else
			throw std::invalid_argument("Unknown probability type.");
		
		return nodeProbabilities;
	}

	void graphski::RandomGraphGenerator::generate(std::vector<float> const& probabilities)
	{
		if(m_graph.nodeCount() == 0)
			return; // cannot generate edges in a graph without nodes
		
		// default to uniform distribution
		const auto& nodeProbabilities = probabilities.empty() ? 
										generateProbabilities(Distributions::Uniform) :
										probabilities;
		
		if(m_graph.nodeCount() != nodeProbabilities.size())
			throw std::invalid_argument("Probabilities size must match the number of nodes.");

		// from every node
		for (NodeId j = 0; j < m_graph.nodeCount(); ++j)
		{
			// to every other node i
			for (NodeId i = 0; i < m_graph.nodeCount(); ++i)
			{
				// add an edge with probability probabilities[i]
				if(distribution(gen) < nodeProbabilities[i])
					m_graph.addEdge(j, i);
			}
		}
	}
}
