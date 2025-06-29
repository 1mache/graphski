#include <SFML/Graphics.hpp>
#include <vector>

class Config
{
	inline static sf::Font s_txtFont = sf::Font("fonts/InriaSans.ttf");
public:
	static const sf::Font& getFont(){ return s_txtFont; }

	static constexpr bool crazyColors = true;
};