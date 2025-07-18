#pragma once
#include "Edge.h"
#include "Node.h"
#include "Arrow.h"
#include "DrawableNode.h"

namespace graphski
{
	class DrawableEdge : public Edge<DrawableNode>, public sf::Drawable
	{
		// mark and selection colors are the same across all nodes
		inline static sf::Color s_idleColor     = Config::IDLE_OUTLINE_COLOR;
		inline static sf::Color s_markedColor   = Config::MARKED_COLOR;
		inline static sf::Color s_selectedColor = Config::SELECTED_COLOR;

		// cached arrow component. needs to be changed in draw function which is const, 
		// this is simply a visual component of the edge. it does not affect the edge logic
		mutable Arrow m_arrow;

	public:
		DrawableEdge(DrawableNode* from, DrawableNode* to) : Edge(from, to),
			m_arrow(from->getPosition(), to->getPosition(), LINE_THICKNESS, s_idleColor)
		{};

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	private:
		static constexpr float LINE_THICKNESS = 5.f;

		// position offset of the self edge circle relative to the node center
		static constexpr sf::Vector2f SELF_EDGE_OFFSET{ -1.f, -1.f };
	};
}