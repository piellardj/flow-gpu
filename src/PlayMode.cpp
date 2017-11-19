#include "PlayMode.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"
#include "glm.hpp"

#include <iostream>
#include <SFML/Graphics/Image.hpp>


PlayMode::PlayMode(std::string const& backgroundFilename, std::string const& flowFilename, sf::Window const& window) :
	Mode(window),
	_flowFilename(flowFilename)
{
	/* Background loading */
	{
		glm::uvec2 bufferSize;
		std::vector<uint8_t> backgroundBuffer;

		if (!IO::load32bitImage(backgroundFilename, bufferSize, backgroundBuffer)) {
			bufferSize = glm::uvec2(1u, 1u);
			backgroundBuffer.resize(4u, 128u);
		}

		_background.reset(new Background(bufferSize, backgroundBuffer));

		std::vector<uint8_t> density(bufferSize.x * bufferSize.y);
		for (unsigned int i = 0u; i < density.size(); ++i) {
			density[i] = backgroundBuffer[4u * i + 3];
		}

		_densityMap.reset(new DensityMap(bufferSize, density));
	}

	/* Flow map loading */
	{
		glm::uvec2 bufferSize;
		std::vector<glm::vec2> buffer;
		IO::loadFlowMap(flowFilename, bufferSize, buffer);
		_flowMap.reset(new FlowMap(bufferSize, buffer));
	}

	std::vector<glm::vec2> initPos = _densityMap->computeInitPos(128*128);
	_particles.reset(new Particles(initPos));
}


void PlayMode::update(float time)
{
	_particles->update(*_flowMap, *_background, time);
}

void PlayMode::display() const
{
	if (_background && showBackground()) {
		_background->display();
	}
	if (_flowMap && showArrows()) {
		_flowMap->drawArrows();
	}

	_particles->draw();
}

void PlayMode::mouseMoved(glm::vec2 const& movement)
{
	_flowMap->addFlow(mousePos(), movement);
}

void PlayMode::doHandleEvent(sf::Event const& event)
{
}