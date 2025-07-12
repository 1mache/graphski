#pragma once
#include "Node.h"

namespace graphski
{
	template<typename NodeT>
	class Edge
	{
		NodeT* m_from = nullptr;
		NodeT* m_to   = nullptr;

		bool m_marked = false;
		
	public:
		// directed by default
		Edge(NodeT* from, NodeT* to): 
			m_from(from), m_to(to){}

		// Note: no destructor, the nodes 
		// will be deleted with the adj list of the graph
		virtual ~Edge() {};
		
		bool operator==(const Edge<NodeT>& other) const
		{
			return (other.m_to == m_to && other.m_from == m_from);
		}

		// marks the node, usually we want to mark true
		virtual void mark(bool val = true) { m_marked = val; }

		bool isMarked() const { return m_marked; }

		NodeT* getFrom() const { return m_from;}

		NodeT* getTo() const { return m_to; }
	};
}