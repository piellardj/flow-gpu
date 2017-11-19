#ifndef MODE_HPP_INCLUDED
#define MODE_HPP_INCLUDED


#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "glm.hpp"


class Mode
{
public:
	Mode(sf::Window const& window);
	virtual ~Mode() {}

	void handleEvent(sf::Event const& event, sf::Window const& window);
	virtual void update(float time) = 0;
	virtual void display() const = 0;

protected:
	inline glm::vec2 mousePos() const { return _fMousePos; }
	inline bool showArrows() const { return _showArrows; }
	inline bool showBackground() const { return _showBackground; }
	inline bool showFlowMap() const { return _showFlowMap; }
	inline bool showBrush() const { return _showBrush; }

private:
	virtual void mouseMoved(glm::vec2 const& movement) = 0;
	virtual void doHandleEvent(sf::Event const& event) = 0;

	void retrieveMousePos(sf::Window const& window);

private:
	glm::ivec2 _iMousePos;
	glm::vec2 _fMousePos;

	bool _showArrows; //A
	bool _showBackground; //B
	bool _showFlowMap; //F
	bool _showBrush; //C
};

#endif // MODE_HPP_INCLUDED