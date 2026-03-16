#pragma once
#include <string>
#include "GraphTypes.h"

namespace graphski 
{
	class Node
	{
	public:
		explicit Node(NodeId id, std::string_view name = "") :m_id(id), m_name(name)
		{
			if (name.size() == 0) // if empty string was passed
				m_name = std::to_string(m_id); //set the name to be the id (as string)
		}
		
		bool operator==(const Node& other) const
		{
			return other.m_id == m_id;
		}

		// =================Degrees===================

		NodeId getDIn() const { return m_dIn; }

		NodeId getDOut() const { return m_dOut; }

		// get the undirected degree
		NodeId getD() const { return m_dIn + m_dOut; };
		
		void setDIn(NodeId val) { m_dIn = val; }

		void setDOut(NodeId val) { m_dOut = val; }
		// ===========================================
		// =================ID and Name===================

		NodeId getId() const { return m_id; }

		std::string getName() const { return m_name; }

		void setName(std::string name) { m_name = name; }
		// ===============================================
		
		Node(const Node&) = default;
		Node& operator=(const Node&) = delete; // TODO: allow later.
		Node(Node&&) = default;
		Node& operator=(Node&&) = default;

		virtual ~Node() = default;
	private:
		// id of the node, should be unique
		NodeId		m_id;
		// if we want to give our node a name like A,B,C
		std::string m_name;

		// in Degree, out Degree
		NodeId		m_dIn = 0, 
					m_dOut = 0;
	};
}