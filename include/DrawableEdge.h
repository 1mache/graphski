#pragma once
#include "Edge.h"
#include "Node.h"
#include "Arrow.h"
#include "DrawableNode.h"

namespace graphski
{
	class DrawableEdge : public Edge, public sf::Drawable
	{
	public:
		DrawableEdge(const DrawableNode* from, const DrawableNode* to) : Edge(from, to),
			m_arrow(from->getPosition(), to->getPosition(), LINE_THICKNESS, s_idleColor)
		{};

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		const DrawableNode* getFrom() const override 
		{ 
			auto* res = dynamic_cast<const DrawableNode*>(Edge::getFrom()); 
			if(!res)
				throw std::runtime_error("\'from\' node is not a DrawableNode.");
			return res;
		}

		const DrawableNode* getTo() const override 
		{ 
			auto* res = dynamic_cast<const DrawableNode*>(Edge::getTo());
			if (!res)
				throw std::runtime_error("\'to\' node is not a DrawableNode.");
			return res;
		}

	private:
		// mark and selection colors are the same across all nodes
		inline static sf::Color s_idleColor     = Config::IDLE_OUTLINE_COLOR;
		inline static sf::Color s_markedColor   = Config::MARKED_COLOR;
		inline static sf::Color s_selectedColor = Config::SELECTED_COLOR;

		// why mutable: it is a cached arrow component. it needs to be changed in draw function which is const, 
		// this is simply a visual component of the edge. it does not affect the edge logic
		mutable Arrow m_arrow;
	
	private:
		static constexpr float LINE_THICKNESS = 5.f;

		// position offset of the self edge circle relative to the node center
		static constexpr sf::Vector2f SELF_EDGE_OFFSET{ -1.f, -1.f };
	};
}