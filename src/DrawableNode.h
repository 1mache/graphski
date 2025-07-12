#pragma once
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Node.h"
#include "Config.h"

namespace graphski
{
	class DrawableNode : public Node, public sf::Drawable, public sf::Transformable
	{
		// visual components
		sf::CircleShape m_circle;
		sf::Text        m_nameTxt;
		sf::Color		m_nodeColor;

		// TODO: this assumes theres only one graph
		// can be changed to be set by specific graph pointer
		// mark and selection colors are the same across all nodes
		inline static sf::Color s_markedColor  = Config::MARKED_COLOR;
		inline static sf::Color s_selectColor  = Config::SELECTED_COLOR;
		inline static sf::Color s_textColor    = Config::TEXT_COLOR; // color when not selected or marked
		inline static sf::Color s_outlineColor = Config::IDLE_OUTLINE_COLOR; // color when not selected or marked

	public:
		DrawableNode(NodeId id, std::string name = "", sf::Color nodeColor = Config::DEFAULT_NODE_COLOR) :
			Node(id, name),
			m_nameTxt(Config::getFont()), m_nodeColor(nodeColor)
		{		
			// TODO: this should be set by screen size	
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

		// when the node is marked 
		void mark(bool val = true) override;
		// when the node is selected (selected on screen by user)
		void select(bool val = true);
		
		// returns bounds of the object based on bounds of circle
		sf::FloatRect getGlobalBounds()
		{
			return getTransform().transformRect(m_circle.getGlobalBounds());
		}

		// set the node color
		void setColor(sf::Color color)
		{
			m_nodeColor = color;
			m_circle.setFillColor(m_nodeColor);
		}

	private:
		// sets up the circle component of the node
		void initCircleComponent();
		// sets up the text component of the node
		void initTextComponent();
		// helper function for mark and select that colors the text and outline of the node
		// according to the boolean value passed to it
		void highlight(sf::Color color)
		{
			m_circle.setOutlineColor(color);
			m_nameTxt.setFillColor(color);
		}

	public: // constants
		static constexpr float        NODE_RADIUS = 30.f;
	private: 
		static constexpr float        NODE_OUTLINE_THICKNSS = 5.f;
		static constexpr unsigned int CIRCLE_RES = 10u;
		static constexpr unsigned int NAME_TEXT_SIZE = 30u; //TODO: these will probably not be constexp
		static constexpr NodeId       MAX_DISPLAYED_CHARS = 3u;
	};
}