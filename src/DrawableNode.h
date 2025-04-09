#pragma once
#include "Node.h"
#include "SFML/Graphics.hpp"
#include <iostream>

namespace graphski
{
	class DrawableNode : public Node, public sf::Drawable, public sf::Transformable
	{
		static constexpr sf::Color    NODE_OUTLINE_COLOR{sf::Color::White};
		static constexpr sf::Color    NODE_COLOR{ 0x5F8B4CFF };
		static constexpr sf::Color    NODE_TEXT_COLOR{sf::Color::White};
		static constexpr float        NODE_RADIUS = 30.f;
		static constexpr float        NODE_OUTLINE_THICKNSS = 5.f;
		static constexpr unsigned int CIRCLE_RES = 10u;
		static constexpr unsigned int NAME_TEXT_SIZE = 30u; //TODO: these will probably not be constexp
		static constexpr uint8_t      MAX_DISPLAYED_CHARS = 3u;

		sf::CircleShape m_circle;
		sf::Text m_nameTxt;

	public:
		DrawableNode(uint8_t id, sf::Vector2f position, sf::Font& txtFont, std::string name = "") : 
			Node(id, name),
			m_circle(NODE_RADIUS, CIRCLE_RES), m_nameTxt(txtFont)
		{		
			//set position of the transform
			setPosition(position);
			setScale({0.8f, 0.8f}); // lower the scale a little, this should be defined by screenSize later

			initCircleComponent();
			initTextComponent();
		}

		virtual ~DrawableNode(){};

		void draw(sf::RenderTarget& target, sf::RenderStates states) const override 
		{
			states.transform *= getTransform(); 
			// apply this objects transform to circle and text
			target.draw(m_circle, states);
			target.draw(m_nameTxt, states);
		}

		// returns bounds of the object based on bounds of circle
		sf::FloatRect getGlobalBounds()
		{
			return getTransform().transformRect(m_circle.getGlobalBounds());
		}

	private:
		// sets up the circle component of the node
		void initCircleComponent();
		// sets up the text component of the node
		void initTextComponent();
	};
}