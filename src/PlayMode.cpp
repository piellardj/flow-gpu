#include "PlayMode.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"
#include "glm.hpp"

#include <iostream>
#include <SFML/Graphics/Image.hpp>


PlayMode::PlayMode(std::vector<LevelFilename> const& lvlFilenames, sf::Window const& window) :
	Mode(window),
	_picking(glm::uvec2(window.getSize().x, window.getSize().y))
{
	for (unsigned int iL = 0u; iL < lvlFilenames.size(); ++iL) {
		_levels.push_back(Level());

		LevelFilename const& names = lvlFilenames[iL];
		Level& lvl = _levels.back();


		/* Background loading */
		{
			glm::uvec2 bufferSize;
			std::vector<uint8_t> backgroundBuffer;

			if (!IO::load32bitImage(names.backgroundFilename, bufferSize, backgroundBuffer)) {
				bufferSize = glm::uvec2(1u, 1u);
				backgroundBuffer.resize(4u, 128u);
			}

			lvl.background.reset(new Background(bufferSize, backgroundBuffer));
			_picking.addBackground(*lvl.background, iL);

			std::vector<uint8_t> density(bufferSize.x * bufferSize.y);
			for (unsigned int i = 0u; i < density.size(); ++i) {
				density[i] = backgroundBuffer[4u * i + 3];
			}

			lvl.densityMap.reset(new DensityMap(bufferSize, density));
		}

		/* Flow map loading */
		{
			glm::uvec2 bufferSize;
			std::vector<glm::vec2> buffer;
			IO::loadFlowMap(names.flowFilename, bufferSize, buffer);
			lvl.flowMap.reset(new FlowMap(bufferSize, buffer));
		}

		std::vector<glm::vec2> initPos = lvl.densityMap->computeInitPos(200*200);
		lvl.particles.reset(new Particles(initPos));
	}

	_picking.lock();
}

void PlayMode::display() const
{
	for (Level const& lvl : _levels) {
		if (lvl.background && showBackground()) {
			lvl.background->display();
		}
	}

	for (unsigned int iL = 0u; iL < _levels.size(); ++iL) {
		_levels[iL].particles->draw(iL);
	}
}

void PlayMode::mouseMoved(glm::vec2 const& movement)
{
	if (_lastLevelPicked < _levels.size()) {
		_levels[_lastLevelPicked].flowMap->addFlow(mousePos(), movement);
	}
	//_picking.getLevel(mousePos());

	/*for (Level const& lvl : _levels) {
		lvl.flowMap->addFlow(mousePos(), movement);
	}*/
}

void PlayMode::doUpdate(float time, float dt)
{
	for (Level const& lvl : _levels) {
		lvl.particles->update(*lvl.flowMap, *lvl.background, time);
	}
}

void PlayMode::doHandleEvent(sf::Event const& event)
{
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == sf::Mouse::Left) {
			_lastLevelPicked = _picking.getLevel(mousePos());
		}
		break;
	default:
		break;
	}
}