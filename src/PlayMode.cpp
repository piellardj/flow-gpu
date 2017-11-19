#include "PlayMode.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"

#include <iostream>
#include <SFML/Graphics/Image.hpp>


PlayMode::PlayMode(std::string const& backgroundFilename, std::string const& flowFilename, sf::Window const& window) :
	Mode(window),
	_flowFilename(flowFilename),
	_particles(256*256)
{
	/* Background loading */
	{
		glm::uvec2 bufferSize;
		std::vector<uint8_t> buffer;

		if (!IO::load24bitImage(backgroundFilename, bufferSize, buffer)) {
			bufferSize = glm::uvec2(1u, 1u);
			buffer.resize(3u, 128u);
		}

		_background.reset(new Background(bufferSize, buffer));
	}

	/* Flow map loading */
	{
		glm::uvec2 bufferSize;
		std::vector<glm::vec2> buffer;
		IO::loadFlowMap(flowFilename, bufferSize, buffer);
		_flowMap.reset(new FlowMap(bufferSize, buffer));
	}
}


void PlayMode::update(float time)
{
	_particles.update(*_flowMap, *_background, time);
}

void PlayMode::display() const
{
	if (_background && showBackground()) {
		_background->display();
	}
	if (_flowMap && showArrows()) {
		_flowMap->drawArrows();
	}

	_particles.draw();
}

void PlayMode::mouseMoved(glm::vec2 const& movement)
{
	_flowMap->addFlow(mousePos(), movement);
}

void PlayMode::doHandleEvent(sf::Event const& event)
{
}