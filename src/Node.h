#pragma once
#include <string>

namespace graphski 
{
	class Node
	{
		inline static uint8_t nodeCount = 0;
		// node id
		uint8_t m_id;
		// if we want to give our node a name like A,B,C
		std::string m_name;
		// is the node marked, used for various algorithms
		bool m_marked = false;

	public:
		explicit Node(std::string name = "") :m_id(nodeCount), m_name(name)
		{
			if (name.size() == 0) // if empty string was passed
				m_name = std::to_string(m_id); //set the name to be the id (as string)
			nodeCount++;
		}

		bool operator==(const Node& other)
		{
			return other.m_id == m_id;
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

		uint8_t getId()
		{
			return m_id;
		}

		std::string getName()
		{
			return m_name;
		}

		void setName(std::string name)
		{
			m_name = name;
		}
	};
}