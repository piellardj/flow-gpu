#include "EditMode.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"

#include <SFML/Graphics/Image.hpp>


EditMode::EditMode(std::string const& saveFilename, sf::Window const& window, std::string const& flowmapFilename):
	Mode(window),
	_saveFilename(saveFilename),
	MIN_BRUSH_SIZE(0.05f),
	MAX_BRUSH_SIZE(0.4f),
	_brushSize(0.5f * (MIN_BRUSH_SIZE + MAX_BRUSH_SIZE))
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
	if (IO::load24bitImage(filename, bufferSize, buffer)) {
		_background.reset(new Background(bufferSize, buffer));
	}
}

void EditMode::update(float time)
{
}

void EditMode::display() const
{
	if (_background && showBackground()) {
		_background->display(0.6f);
	}
	if (showFlowMap()) {
		_flowMap->drawMap();
	}
	if (showArrows()) {
		_flowMap->drawArrows();
	}
	if (showBrush()) {
		_brush.display(mousePos(), glm::vec2(_brushSize));
	}
}

void EditMode::mouseMoved(glm::vec2 const& movement)
{
	_flowMap->addFlow(mousePos(), movement, _brushSize);
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
	case sf::Event::MouseWheelScrolled:
	{
		float dSize = (MAX_BRUSH_SIZE - MIN_BRUSH_SIZE) / 10.f;
		_brushSize -= dSize * static_cast<float>(event.mouseWheelScroll.delta);
		_brushSize = std::min(MAX_BRUSH_SIZE, std::max(MIN_BRUSH_SIZE, _brushSize));
	}
		break;
	default:
		break;
	}
}