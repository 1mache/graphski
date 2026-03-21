#pragma once
#include <string>
#include "GraphTypes.h"

namespace graphski 
{

	/* Base class with no data for testing, visualization and
	other purposes where the structure of the graph is all we care about */
	class Node
	{
	public:
		explicit Node(std::string_view name = "") : m_name(name)
		{}
		virtual ~Node() = default;
		
		
		// =================Degrees===================
		Degree getDegIn() const { return m_dIn; }

		Degree getDegOut() const { return m_dOut; }

		// get the undirected degree
		Degree getDegree() const { return m_dIn + m_dOut; };
		
		void setDegIn(Degree val) { m_dIn = val; }

		void setDOut(Degree val) { m_dOut = val; }
		// ===========================================
		// =================ID and Name===================

		const std::string& getName() const { return m_name; }

		void setName(std::string_view name) { m_name = name; }
		// ===============================================
	private:
		std::string m_name;

		// in Degree, out Degree
		Degree		m_dIn = 0, 
					m_dOut = 0;
	};
}