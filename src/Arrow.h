#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

namespace graphski
{
	class Arrow : public sf::Drawable
	{
		sf::Vector2f m_from;
		sf::Vector2f m_to;
		// the offset is how much length will be subtracted
		// from the arrow. (from the end)
		float m_headOffset = 0.f;

		float	  m_thickness;
		sf::Color m_color;

	public:
		Arrow(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color) :
			m_from(from), m_to(to), m_thickness(thickness), m_color(color) {}

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		
		// setter for the offset
		void setHeadOffset(float value) { m_headOffset = value; }
	private:
		static constexpr float	   HAIR_LEN  = 20.f;
		static constexpr sf::Angle HAIR_ANGLE = sf::degrees(30);
	};
}