#include "RandomGraphGenerator.h"

namespace graphski
{
	void RandomGraphGenerator::setProbabilities(Distributions type)
	{
		if (type == Distributions::Uniform)
			m_nodeProbabilities.assign(m_nodeCount, 1.0f / m_nodeCount);

		else if (type == Distributions::IndexDependent)
		{
			m_nodeProbabilities.clear();
			for (NodeId i = 0; i < m_nodeCount; ++i)
				m_nodeProbabilities.push_back(1.0f / (i + 1));
		}

		else
			throw std::invalid_argument("Unknown probability type.");
	}

	void graphski::RandomGraphGenerator::generate()
	{
		if (m_nodeProbabilities.size() == 0)
			setProbabilities(Distributions::Uniform); // default to uniform distribution
		
		std::vector<NodeId> generatedNodeIds;
		generatedNodeIds.reserve(m_nodeCount);		

		for (NodeId i = 0; i < m_nodeCount; ++i)
			generatedNodeIds.push_back(m_graph.addNode());

		// from every node
		for (NodeId j = 0; j < m_nodeCount; ++j)
		{
			// to every other node i
			for (NodeId i = 0; i < m_nodeCount; ++i)
			{
				// add an edge with probability m_nodeProbabilities[i]
				if(distribution(gen) < m_nodeProbabilities[i])
					m_graph.addEdge(generatedNodeIds[j], generatedNodeIds[i]);
			}
		}
	}
}
