#pragma once

#include <string>
#include "GraphTypes.h"
#include "Node.h"

namespace graphski 
{
    template <typename T>
	class DataNode : public Node
	{
	public:
		explicit DataNode(NodeId id, T data, std::string_view name = "") :
			m_data(data), m_id(id), m_name(name)
		{
			if (name.size() == 0) // if empty string was passed
				m_name = std::to_string(m_id); //set the name to be the id (as string)
		}

		// =================Data======================
		// by const ref when we want to look at the data but not take it out of the node
		const T& getData() const { return m_data; }
		// by value when we want to "take out" the data and not care about the node anymore
		const T  retrieveData() const { return m_data; }

		// =================Degrees===================

		Degree getDegIn() const { return m_dIn; }

		Degree getDegOut() const { return m_dOut; }

		// get the undirected degree
		Degree getDegree() const { return m_dIn + m_dOut; };
		
		void setDIn(Degree val) { m_dIn = val; }

		void setDOut(Degree val) { m_dOut = val; }
		// ===========================================
		// =================ID and Name===================

		Degree getId() const { return m_id; }

		const std::string& getName() const { return m_name; }

		void setName(std::string_view name) { m_name = name; }
		// ===============================================
		
		virtual ~DataNode() = default;
	private:
		// the data of the node, can be anything
		const T     m_data; 
		// id of the node, should be unique
		NodeId		m_id;
		// if we want to give our node a name like A,B,C
		std::string m_name;

		// in Degree, out Degree
		Degree		m_dIn = 0, 
					m_dOut = 0;
	};
}