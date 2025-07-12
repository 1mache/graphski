#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Config
{

	inline static sf::Font s_txtFont = sf::Font("fonts/InriaSans.ttf");
public:
	static constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;

	static const sf::Font& getFont(){ return s_txtFont; }

	static constexpr bool crazyColors = true;

	// Colors
	static constexpr sf::Color DEFAULT_NODE_COLOR{ 0xFFB200FF };
	static constexpr sf::Color IDLE_OUTLINE_COLOR{ sf::Color::White };
	static constexpr sf::Color MARKED_COLOR{ 0xD91656FF };
	static constexpr sf::Color SELECTED_COLOR{ 0xD91656FF };
	static constexpr sf::Color TEXT_COLOR{ sf::Color::White };
};