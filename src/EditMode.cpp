#include "EditMode.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"

#include <SFML/Graphics/Image.hpp>


EditMode::EditMode(std::string const& saveFilename, sf::Window const& window, std::string const& flowmapFilename):
	Mode(window),
	_saveFilename(saveFilename)
{
	glm::uvec2 bufferSize(128, 128);
	std::vector<glm::vec2> buffer(bufferSize.x * bufferSize.y, glm::vec2(0.f));

	if (flowmapFilename != "") {
		if (!IO::loadFlowMap(flowmapFilename, bufferSize, buffer)) {
			bufferSize = glm::uvec2(128u, 128u);
			buffer.resize(bufferSize.x * bufferSize.y, glm::vec2(0.f));
		}
	}

	_flowMap.reset(new FlowMap(bufferSize, buffer));
}

void EditMode::setBackground(std::string const& filename)
{
	glm::uvec2 bufferSize;
	std::vector<uint8_t> buffer;
	if (IO::load32bitImage(filename, bufferSize, buffer)) {
		_background.reset(new Background(bufferSize, buffer));
	}
}

#include <iostream>

void EditMode::display() const
{
	if (_background && showBackground()) {
		_background->display(0.6f);
	}
	if (showFlowMap()) {
		_flowMap->drawMap();
	}
	if (showArrows()) {
		_flowMap->drawArrows(_screenSize);
	}
	if (showBrush()) {
		brush().display(_screenSize, iMousePos(), true);
	}
}

void EditMode::mouseMoved(glm::ivec2 const& movement)
{
	glm::vec2 relativeMovement = glm::vec2(movement.x, movement.y) / glm::vec2(_screenSize.x, _screenSize.y);
	glm::vec2 relativeBrushSize = 2.f * glm::vec2(brush().radius()) / glm::vec2(_screenSize.x, _screenSize.y);

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		_flowMap->changeLocally(mousePos(), relativeMovement, relativeBrushSize);
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		_flowMap->changeLocally(mousePos(), relativeMovement, relativeBrushSize, true);
	}
}

void EditMode::doUpdate(float time, float dt)
{
}

void EditMode::doHandleEvent(sf::Event const& event)
{
	switch (event.type) {
	case sf::Event::KeyPressed:
		if (event.key.code == sf::Keyboard::S) {
			glm::uvec2 bufferSize;
			std::vector<glm::vec2> buffer;
			_flowMap->getData(bufferSize, buffer);

			IO::saveFlowMap(_saveFilename, bufferSize, buffer);
		}
	default:
		break;
	}
}