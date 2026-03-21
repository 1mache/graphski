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
		virtual ~DataNode() = default;

		// =================Data======================
		// by const ref when we want to look at the data but not take it out of the node
		const T& getData() const { return m_data; }
		// by value when we want to "take out" the data and not care about the node anymore
		const T  retrieveData() const { return m_data; }
		
	private:
		// the data of the node, can be anything
		const T m_data; 
	};
}