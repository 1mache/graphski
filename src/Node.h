#pragma once
#include <string>

namespace graphski 
{
	class Node
	{
		// id of the node, should be unique
		uint8_t m_id;
		// if we want to give our node a name like A,B,C
		std::string m_name;
		// is the node marked, used for various algorithms
		bool m_marked = false;

		// in Degree, out Degree
		uint8_t m_dIn = 0, m_dOut = 0;

	public:
		explicit Node(uint8_t id, std::string name = "") :m_id(id), m_name(name)
		{
			if (name.size() == 0) // if empty string was passed
				m_name = std::to_string(m_id); //set the name to be the id (as string)
		}

		bool operator==(const Node& other) const
		{
			return other.m_id == m_id;
		}

		// =================Marking===================
		
		// marks the node, usually we want to mark true
		bool mark(bool val = true) { m_marked = val; }

		bool isMarked() const { return m_marked; }
		// ===========================================
		// =================Degrees===================

		uint8_t getDIn() const { return m_dIn; }

		uint8_t getDOut() const { return m_dOut; }

		// get the undirected degree
		uint8_t getD() const { return m_dIn + m_dOut; };
		
		void setDIn(uint8_t val) { m_dIn = val; }

		void setDOut(uint8_t val) { m_dOut = val; }
		// ===========================================
		// =================ID and Name===================

		uint8_t getId() const { return m_id; }

		std::string getName() const { return m_name; }

		void setName(std::string name) { m_name = name; }
		// ===============================================
	};
}