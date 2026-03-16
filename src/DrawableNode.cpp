#include "DrawableNode.h"

namespace graphski
{
	void DrawableNode::mark(bool val)
	{
		m_marked = val;
		if (val)
			highlight(s_markedColor);
		else
			if(m_selected)
				highlight(s_selectColor);
			else
				highlight(s_outlineColor);
	}

	void DrawableNode::toggleSelect()
	{
		if (m_selected)
			highlight(s_outlineColor);
		else
			highlight(s_selectColor);

		m_selected = !m_selected;
	}

	void DrawableNode::initCircleComponent()
	{
		// set circle colors
		m_circle = sf::CircleShape(NODE_RADIUS, CIRCLE_RES);
		m_circle.setFillColor(m_nodeColor);
		m_circle.setOutlineThickness(NODE_OUTLINE_THICKNSS);
		m_circle.setOutlineColor(s_outlineColor);

		// set origins to center components:
		sf::Vector2f circleCenterOffset(NODE_RADIUS, NODE_RADIUS);
		m_circle.setOrigin(circleCenterOffset);
	}

	void DrawableNode::initTextComponent()
	{
		//set text properties
		m_nameTxt.setFillColor(s_textColor);
		m_nameTxt.setCharacterSize(NAME_TEXT_SIZE);
		// displays a max of 3 chars out of the node name
		m_nameTxt.setString(Node::getName().substr(0, MAX_DISPLAYED_CHARS));

		// find text bounding box center
		auto textBoxCenter = m_nameTxt.getGlobalBounds().getCenter();
		// round it, text is better displayed like this
		m_nameTxt.setOrigin(textBoxCenter);
	}
}