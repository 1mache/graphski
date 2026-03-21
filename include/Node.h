#pragma once
#include <string>
#include "GraphTypes.h"
#include "INode.h"

namespace graphski 
{

	/* Base class with no data for testing, visualization and
	other purposes where the structure of the graph is all we care about */
	class Node : public INode
	{
	public:
		explicit Node(std::string_view name = "") : m_name(name)
		{}
		virtual ~Node() = default;
		
		
		// =================Degrees===================
		Degree getDegIn() const override { return m_dIn; }

		Degree getDegOut() const override { return m_dOut; }
		
		void setDegIn(Degree val) override { m_dIn = val; }

		void setDegOut(Degree val) override { m_dOut = val; }
		// ===========================================
		// =================Name===================

		const std::string& getName() const override { return m_name; }

		void setName(std::string_view name) override { m_name = name; }
		// ===============================================
	private:
		std::string m_name;

		// in Degree, out Degree
		Degree		m_dIn = 0, 
					m_dOut = 0;
	};
}