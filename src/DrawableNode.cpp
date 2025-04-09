#include "DrawableNode.h"

namespace graphski
{
	void DrawableNode::initCircleComponent()
	{
		// set circle colors
		m_circle.setFillColor(NODE_COLOR);
		m_circle.setOutlineThickness(NODE_OUTLINE_THICKNSS);
		m_circle.setOutlineColor(NODE_OUTLINE_COLOR);

		// set origins to center components:
		sf::Vector2f circleCenterOffset(NODE_RADIUS, NODE_RADIUS);
		m_circle.setOrigin(circleCenterOffset);
	}

	void DrawableNode::initTextComponent()
	{
		//set text properties
		m_nameTxt.setFillColor(NODE_TEXT_COLOR);
		m_nameTxt.setCharacterSize(NAME_TEXT_SIZE);
		// displays a max of 3 chars out of the node name
		m_nameTxt.setString(Node::getName().substr(0, MAX_DISPLAYED_CHARS));

		// find text bounding box center
		auto textBoxCenter = m_nameTxt.getGlobalBounds().getCenter();
		// round it, text is better displayed like this
		m_nameTxt.setOrigin(textBoxCenter);
	}
}