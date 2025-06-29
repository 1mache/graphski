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

	public:
		DrawableEdge(DrawableNode* from, DrawableNode* to) : Edge(from, to) {};

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	private:
		static constexpr float LINE_THICKNESS = 5.f;
	};
}