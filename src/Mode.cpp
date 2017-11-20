#include "Mode.hpp"


Mode::Mode(sf::Window const& window):
	_showArrows(true),
	_showBackground(true),
	_showFlowMap(true),
	_showBrush(true)
{
	retrieveMousePos(window);
}

void Mode::handleEvent(sf::Event const& event, sf::Window const& window)
{
	glm::ivec2 prevPos = _iMousePos;
	retrieveMousePos(window);
	if (_iMousePos != prevPos && sf::Mouse::isButtonPressed(sf::Mouse::Left) && window.hasFocus()) {
		glm::ivec2 movement = _iMousePos - prevPos;
		glm::vec2 fWindow(window.getSize().x, window.getSize().y);
		glm::vec2 fMovement = glm::vec2(movement.x, movement.y) / fWindow;

		mouseMoved(fMovement);
	}

	switch (event.type) {
	case sf::Event::KeyPressed:
		if (event.key.code == sf::Keyboard::A) {
			_showArrows = !_showArrows;
		}
		else if (event.key.code == sf::Keyboard::F) {
			_showFlowMap = !_showFlowMap;
		}
		else if (event.key.code == sf::Keyboard::B) {
			_showBackground = !_showBackground;
		}
		else if (event.key.code == sf::Keyboard::C) {
			_showBrush = !_showBrush;
		}
		else {
			doHandleEvent(event);
		}
		break;
	break;
	default:
		doHandleEvent(event);
		break;
	}
}

void Mode::update(float time)
{
	doUpdate(time, time - _lastUpdate);
	_lastUpdate = time;
}

void Mode::retrieveMousePos(sf::Window const& window)
{
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	_iMousePos = glm::ivec2(pos.x, static_cast<int>(window.getSize().y) - pos.y);
	_fMousePos = glm::vec2(_iMousePos.x, _iMousePos.y) / glm::vec2(window.getSize().x, window.getSize().y);
}