#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>

#include <SFML/Graphics.hpp>

class GraphicsConfig
{
public:
	static constexpr unsigned int WINDOW_WIDTH = 800u, WINDOW_HEIGHT = 600u;

	static constexpr bool crazyColors = true;

	static const sf::Font& getFont()
	{
		static bool initialized = false;
		static sf::Font s_font;
		if(!std::filesystem::exists(FONT_PATH))
			throw std::runtime_error("Font file not found at: " + std::string(FONT_PATH));

		if(!initialized)
		{
			if(!s_font.openFromFile(FONT_PATH))
				throw std::runtime_error("Failed to load font from file: " + std::string(FONT_PATH));
			initialized = true;
		}

		return s_font;
	}

	// Font path
	static constexpr const char* FONT_PATH = "fonts/InriaSans.ttf";

	// Colors
	static constexpr sf::Color DEFAULT_NODE_COLOR{ 0xFFB200FF };
	static constexpr sf::Color IDLE_OUTLINE_COLOR{ sf::Color::White };
	static constexpr sf::Color MARKED_COLOR{ 0xD91656FF };
	static constexpr sf::Color SELECTED_COLOR{ 0x1656D9FF };
	static constexpr sf::Color TEXT_COLOR{ sf::Color::White };
};