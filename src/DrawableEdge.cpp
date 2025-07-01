#include "DrawableEdge.h"

void graphski::DrawableEdge::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// update arrows 'to' and 'from' positions
	m_arrow.setFrom(getFrom()->getPosition());
	m_arrow.setTo(getTo()->getPosition());

	// set the offset so that the arrow hits the side of the node
	// + a little extra to make it look better
	m_arrow.setHeadOffset(DrawableNode::NODE_RADIUS * 0.9f);
	m_arrow.draw(target, states);
}