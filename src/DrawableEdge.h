#pragma once
#include "Edge.h"
#include "Node.h"
#include "DrawableNode.h"

namespace graphski
{
	class DrawableEdge : Edge, sf::Drawable
	{
	public:
		DrawableEdge(DrawableNode* from, DrawableNode* to) : Edge(from, to) {};
		virtual ~DrawableEdge(){};

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override
		{
			// we know they can be casted since we got them as pointers to DrawableNode
			sf::Vector2f fromPos = dynamic_cast<DrawableNode*>(getFrom())->getPosition();
			sf::Vector2f toPos =   dynamic_cast<DrawableNode*>(getTo())->getPosition();
			drawLine(fromPos, toPos);
		}

	private:
		void drawLine(sf::Vector2f lineStart, sf::Vector2f lineEnd) const;
	};
}