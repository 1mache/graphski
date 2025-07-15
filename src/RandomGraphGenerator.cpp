#include "RandomGraphGenerator.h"

namespace graphski
{
	void RandomGraphGenerator::setProbabilities(Distributions type)
	{
		if (type == Distributions::Uniform)
			m_nodeProbabilities.assign(m_nodeCount, 1.0f / m_nodeCount);

		else if (type == Distributions::iDependent)
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


		for (NodeId i = 0; i < m_nodeCount; ++i)
			m_nodes.push_back(m_graph.addNode());

		for (NodeId j = 0; j < m_nodeCount; ++j)
		{
			for (NodeId i = 0; i < m_nodeCount; ++i)
			{
				if(distribution(gen) < m_nodeProbabilities[i])
					m_graph.addEdge(m_nodes[j], m_nodes[i]);
			}
		}
	}
}
