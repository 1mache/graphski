#include "Graph.h"
#include "DrawableNode.h"
#include "DrawableEdge.h"

namespace graphski
{
	class DrawableGraph : public Graph, public sf::Drawable
	{
		sf::Font txtFont;

	public:
		DrawableGraph(sf::Font&& font) : Graph(), txtFont(std::move(font))
		{}
		
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		// creates a node with empty edges list at given position, returns its unique id
		// note: blocks Graph::addNode because we cant create a drawable node without position
		uint8_t addNode(sf::Vector2f position, std::string name = "");

		// creates an edge between to given nodes, gets them by ids
		void addEdge(uint8_t fromNodeId, uint8_t toNodeId) override;

		// checks if a position is in a certain node, returns its id if it is
		std::optional<uint8_t> posInNode(sf::Vector2f position);
	private:
		//TODO: make nodes take color data from here
		static constexpr sf::Color    NODE_COLOR{ 0xFFB200FF };
		static constexpr sf::Color    NODE_OUTLINE_COLOR{ sf::Color::White };
		static constexpr sf::Color	  NODE_MARKED_COLOR{ 0xD91656FF };
		static constexpr sf::Color    NODE_TEXT_COLOR{ sf::Color::White };
	};
}