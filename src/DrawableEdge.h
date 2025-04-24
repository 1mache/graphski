#pragma once
#include "Edge.h"
#include "Node.h"
#include "DrawableNode.h"

namespace graphski
{
	class DrawableEdge : public Edge, public sf::Drawable
	{
	public:
		DrawableEdge(DrawableNode* from, DrawableNode* to) : Edge(from, to) {};
		virtual ~DrawableEdge(){};

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