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
		float m_offset = 0.f;

		float	  m_thickness;
		sf::Color m_color;

	public:
		Arrow(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color) :
			m_from(from), m_to(to), m_thickness(thickness), m_color(color) {}

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		
		// setter for the offset
		void setOffset(float value)
		{
			if((m_to - m_from).length() < value)
				throw std::out_of_range("Offset is bigger than the arrow length!");
			else
				m_offset = value; 
		}
	private:
		static constexpr float	   HAIR_LEN  = 25.f;
		static constexpr sf::Angle HAIR_ANGLE = sf::degrees(45);
	};
}