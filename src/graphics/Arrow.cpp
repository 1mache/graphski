#include "graphics/Arrow.h"

namespace graphski::graphics
{
void Arrow::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if(m_from == m_to)
	{
		drawSelfArrow(target, states);
		return;
	}
	
	// vector that represents the big line
	sf::Vector2f lineVector = m_to - m_from;

	float arrowLength = lineVector.length();
	if (arrowLength <= 0.01f)
		return; // arrow length is close to 0, so we don't draw anything

	// shorten the line by the offset
	lineVector *= ((arrowLength  - m_headOffset) / arrowLength);
	// if there is offset m_to is not the end of the arrow
	sf::Vector2f arrowEnd = m_from + lineVector;
	arrowLength -= m_headOffset;

	// create the arrow body
	sf::RectangleShape line({ arrowLength, m_thickness});

	// set the visual components of the arrow body
	line.setFillColor(m_color);
	line.setPosition(m_from);
	// tweak the origin slightly to account for the thickness of the line
	line.setOrigin({ 0.f, m_thickness / 2.f });
	line.rotate(lineVector.angle());

	// lets call the arrow head halves hair
	sf::RectangleShape hair1({ HAIR_LEN, m_thickness });

	// set the visual components of the hair
	hair1.setFillColor(m_color);
	hair1.setOrigin({ HAIR_LEN, m_thickness / 2.f });
	hair1.setPosition(arrowEnd);

	// hair 2 is a copy of hair 1, but rotated in the opposite direction
	sf::RectangleShape hair2 = hair1;
	hair1.rotate(lineVector.angle() + HAIR_ANGLE);
	hair2.rotate(lineVector.angle() - HAIR_ANGLE);

	// draw everything
	target.draw(line,  states);
	target.draw(hair1, states);
	target.draw(hair2, states);
}

void Arrow::drawSelfArrow(sf::RenderTarget& target, sf::RenderStates states) const
{
	sf::CircleShape selfArrow(m_selfArrowRadius, CIRCLE_RES);

	selfArrow.setFillColor(sf::Color::Transparent);
	selfArrow.setOutlineColor(m_color);
	
	selfArrow.setOutlineThickness(m_thickness);

	auto centerOffset = sf::Vector2f{ m_selfArrowRadius, m_selfArrowRadius };
	selfArrow.setOrigin(centerOffset);

	selfArrow.setPosition(m_from + m_selfArrowOffset);

	target.draw(selfArrow, states);
}
}