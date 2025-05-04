#include "DrawableEdge.h"

sf::RectangleShape graphski::DrawableEdge::makeLine(sf::Vector2f lineStart, sf::Vector2f lineEnd) const
{
	// vector that represents the line
	sf::Vector2f lineVector = lineEnd - lineStart;
	sf::RectangleShape line({ lineVector.length(), LINE_THICKNESS });

	line.getFillColor() = s_idleColor;
	line.setPosition(lineStart);
	line.rotate(lineVector.angle());
	return line;
}