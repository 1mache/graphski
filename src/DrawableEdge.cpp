#include "DrawableEdge.h"

void graphski::DrawableEdge::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// we know they can be casted since we got them as pointers to DrawableNode
	sf::Vector2f fromPos = getFrom()->getPosition();
	sf::Vector2f toPos = getTo()->getPosition();
	// TODO: optimize. dont create a new Arrow every time we draw
	auto arrow = Arrow(fromPos, toPos, LINE_THICKNESS, s_idleColor);
	// set the offset so that the arrow hits the side of the node
	// + a little extra to make it look better
	arrow.setHeadOffset(DrawableNode::NODE_RADIUS * 0.9f);
	arrow.draw(target, states);
}