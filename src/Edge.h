#pragma once
#include "Node.h"


namespace graphski
{
	class Edge
	{
		// the edge is directed if this is null,
		// but if its undirected it will point to (to -> from) edge
		Edge* m_twin = nullptr;
		
		Node* m_from = nullptr;
		Node* m_to   = nullptr;

		bool m_marked = false;
		
	public:
		// directed by default
		Edge(Node* from, Node* to): 
			m_from(from), m_to(to){}
		// Note: no destructor, the nodes and twin(if exists)
		// will be deleted with the adj list of the graph
		
		bool operator==(const Edge& other) const
		{
			return (other.m_to == m_to && other.m_from == m_from)
				|| (m_twin && other == *m_twin);
		}

		// marks the node, usually we want to mark true
		bool mark(bool val = true)
		{
			m_marked = val;
		}

		bool isMarked() const
		{
			return m_marked;
		}

		Node* getFrom()
		{
			return m_from;
		}

		Node* getTo()
		{
			return m_to;
		}

		Edge* getTwin()
		{
			return m_twin;
		}

		void setTwin(Edge* twin)
		{
			// check that the twin is not null and is really a twin
			if (twin && *twin == Edge(m_to, m_from))
			{
				m_twin = twin;
				// the edge is now undirected 
			}
		}
	};
}