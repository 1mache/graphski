#pragma once
#include "Node.h"

namespace graphski
{
	class Edge
	{
	public:
		// directed by default
		Edge(const Node* from, const Node* to): 
			m_from(from), m_to(to){}

		// Note: no destructor, the nodes 
		// will be deleted with the adj list of the graph
		virtual ~Edge() {};
		
		bool operator==(const Edge& other) const
		{
			return (other.m_to == m_to && other.m_from == m_from);
		}

		virtual const Node* getFrom() const { return m_from;}

		virtual const Node* getTo() const { return m_to; }
	private:
		const Node* m_from = nullptr;
		const Node* m_to   = nullptr;
	};
}