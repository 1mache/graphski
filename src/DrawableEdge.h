#pragma once
#include "Edge.h"
#include "Node.h"
#include "DrawableNode.h"

namespace graphski
{
	class DrawableEdge : public Edge, public sf::Drawable
	{
		// mark and selection colors are the same across all nodes
		inline static sf::Color s_markedColor = sf::Color::White;
		inline static sf::Color s_selectColor = sf::Color::White;

		sf::Color m_color;

	public:
		DrawableEdge(DrawableNode* from, DrawableNode* to, sf::Color color) : Edge(from, to), m_color(color) {};

		static void setMarkedColor(sf::Color color) { s_markedColor = color; }
		static void setSelectColor(sf::Color color) { s_markedColor = color; }

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// we know they can be casted since we got them as pointers to DrawableNode
			sf::Vector2f fromPos = dynamic_cast<DrawableNode*>(getFrom())->getPosition();
			sf::Vector2f toPos =   dynamic_cast<DrawableNode*>(getTo())->getPosition();
			target.draw(makeLine(fromPos, toPos), states);
		}

	private:
		sf::RectangleShape makeLine(sf::Vector2f lineStart, sf::Vector2f lineEnd) const;

	private:
		static constexpr float LINE_THICKNESS = 5.f;
	};
}