#ifndef MODE_HPP_INCLUDED
#define MODE_HPP_INCLUDED


#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>

#include "glm.hpp"
#include "Brush.hpp"


class Mode
{
public:
	Mode(sf::Window const& window);
	virtual ~Mode() {}

	void handleEvent(sf::Event const& event, sf::Window const& window);
	void update(float time);
	virtual void display() const = 0;

protected:
	glm::vec2 pixelToRelative(glm::ivec2 const& v) const;

	inline glm::ivec2 iMousePos() const { return _iMousePos; }
	inline glm::vec2 mousePos() const { return pixelToRelative(_iMousePos); }

	inline bool showArrows() const { return _showArrows; }
	inline bool showBackground() const { return _showBackground; }
	inline bool showFlowMap() const { return _showFlowMap; }
	inline bool showBrush() const { return _showBrush; }

	inline Brush const& brush() const { return _brush; }

private:
	virtual void mouseMoved(glm::ivec2 const& movement) = 0;
	virtual void doUpdate(float time, float dt) = 0;
	virtual void doHandleEvent(sf::Event const& event) = 0;

	void retrieveMousePos(sf::Window const& window);

protected:
	const glm::uvec2 _screenSize;

private:
	glm::ivec2 _iMousePos;

	Brush _brush;

	float _lastUpdate;

	bool _showArrows; //A
	bool _showBackground; //B
	bool _showFlowMap; //F
	bool _showBrush; //C
};

#endif // MODE_HPP_INCLUDED