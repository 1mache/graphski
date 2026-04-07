#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

namespace graphski::graphics
{
	class Arrow : public sf::Drawable
	{
	public:
		Arrow(sf::Vector2f from, sf::Vector2f to, float thickness, sf::Color color):
			m_from(from), m_to(to), m_color(color), m_thickness(thickness)
		{}

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		
		void drawSelfArrow(sf::RenderTarget& target, sf::RenderStates states) const;

		// setter for the offset
		void setHeadOffset(float value) { m_headOffset = value; }
		
		void setSelfArrowRadius(float radius) 
		{
			if (radius < 0.f)
				throw std::invalid_argument("Self arrow radius cannot be negative");

			m_selfArrowRadius = radius; 
		}

		void setSelfArrowOffset(sf::Vector2f offset)
		{
			m_selfArrowOffset = offset;
		}

		// setters for the from and to points
		void setFrom(sf::Vector2f from) { m_from = from; }
		void setTo(sf::Vector2f to) { m_to = to; }

		// setter for the color
		void setColor(sf::Color color) { m_color = color; }

	private:
		sf::Vector2f m_from;
		sf::Vector2f m_to;
		// the offset is how much length will be subtracted
		// from the arrow. (from the end)
		float	  m_headOffset = 0.f;
		sf::Color m_color;

		float	     m_thickness;
		float		 m_selfArrowRadius = 0.f;
		sf::Vector2f m_selfArrowOffset{ 0.f, 0.f };

	private:
		static constexpr float	      HAIR_LEN  = 20.f;
		static constexpr sf::Angle    HAIR_ANGLE = sf::degrees(30);
		static constexpr sf::Vector2  SELF_ARROW_OFFSET_DIR{-1, 1};
		static constexpr unsigned int CIRCLE_RES = 15u;
	};
}