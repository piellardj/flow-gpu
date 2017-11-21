#include "Scene.hpp"


Scene::Scene(sf::Window const& window) :
	_screenSize(window.getSize().x, window.getSize().y),
	_brush(10.f, 100.f),
	_showArrows(true),
	_showBackground(true),
	_showParticles(true),
	_showFlowMap(true),
	_showBrush(true)
{
	retrieveMousePos(window);
}

void Scene::handleEvent(sf::Event const& event, sf::Window const& window)
{
	glm::ivec2 prevPos = _iMousePos;
	retrieveMousePos(window);
	if (_iMousePos != prevPos && window.hasFocus()) {
		glm::ivec2 movement = _iMousePos - prevPos;

		mouseMoved(movement);
	}

	switch (event.type) {
	case sf::Event::KeyPressed:
		if (event.key.code == sf::Keyboard::A) {
			_showArrows = !_showArrows;
		}
		else if (event.key.code == sf::Keyboard::B) {
			_showBackground = !_showBackground;
		}
		else if (event.key.code == sf::Keyboard::C) {
			_showBrush = !_showBrush;
		}
		else if (event.key.code == sf::Keyboard::F) {
			_showFlowMap = !_showFlowMap;
		}
		else if (event.key.code == sf::Keyboard::P) {
			_showParticles = !_showParticles;
		}
		else {
			doHandleEvent(event);
		}
		break;
	break;
	case sf::Event::MouseWheelScrolled:
		_brush.changeRadius(event.mouseWheelScroll.delta);
		break;
	default:
		doHandleEvent(event);
		break;
	}
}

void Scene::update(float time)
{
	doUpdate(time, time - _lastUpdate);
	_lastUpdate = time;
}

glm::vec2 Scene::pixelToRelative(glm::ivec2 const& v) const
{
	return glm::vec2(v.x, v.y) / glm::vec2(_screenSize.x, _screenSize.y);
}

void Scene::retrieveMousePos(sf::Window const& window)
{
	sf::Vector2i pos = sf::Mouse::getPosition(window);
	_iMousePos = glm::ivec2(pos.x, static_cast<int>(window.getSize().y) - pos.y);
}