#include "DrawableEdge.h"

void graphski::DrawableEdge::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// update arrows 'to' and 'from' positions
	m_arrow.setFrom(getFrom()->getPosition());
	m_arrow.setTo(getTo()->getPosition());

	// self edge case
	if (getFrom() == getTo())
	{
		m_arrow.setSelfArrowRadius(DrawableNode::NODE_RADIUS / 2);
		m_arrow.setSelfArrowOffset(DrawableNode::NODE_RADIUS * SELF_EDGE_OFFSET.normalized());
		m_arrow.drawSelfArrow(target, states);
		return;
	}

	// set the offset so that the arrow hits the side of the node
	float offsetExtra = 0.9f; // + shorten it a bit to make it look better
	m_arrow.setHeadOffset(DrawableNode::NODE_RADIUS * offsetExtra);

	m_arrow.draw(target, states);
}