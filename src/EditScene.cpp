#include "EditScene.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"
#include "DensityMap.hpp"

#include <SFML/Graphics/Image.hpp>
#include <iostream>


EditScene::EditScene(Description const& description, sf::Window const& window) :
	Scene(window),
	_saveFilename(description.saveFilename)
{
	/* Flowmap loading */
	{
		glm::uvec2 flowSize;
		std::vector<glm::vec2> flowBuffer;

		if (IO::loadFlowMap(description.flowmapFilename, flowSize, flowBuffer)) {
			std::cout << "Loaded flow map '" << description.flowmapFilename << "'" << std::endl;
		}
		else {
			flowSize = description.defaultResolution;
			flowSize = glm::uvec2(std::max(flowSize.x, 1u), std::max(flowSize.y, 1u));
			flowBuffer.resize(flowSize.x * flowSize.y, glm::vec2(0.f));
		}

		_flowMap.reset(new FlowMap(flowSize, flowBuffer));
	}

	/* Background and particles loading*/
	{
		glm::uvec2 backgroundSize;
		std::vector<uint8_t> backgroundBuffer;
		if (IO::load32bitImage(description.backgroundFilename, backgroundSize, backgroundBuffer)) {
			std::cout << "Loaded background '" << description.backgroundFilename << "'" << std::endl;

			_background.reset(new Background(backgroundSize, backgroundBuffer));

			std::vector<uint8_t> densityBuffer(backgroundSize.x * backgroundSize.y);
			for (unsigned int i = 0u; i < densityBuffer.size(); ++i) {
				densityBuffer[i] = backgroundBuffer[4u * i + 3];
			}

			DensityMap densityMap(backgroundSize, densityBuffer);
			std::vector<glm::vec2> initPos = densityMap.sample(200 * 200);
			_particles.reset(new Particles(initPos));
			std::cout << "Generated " << _particles->nbParticles() << " particles\n" << std::endl;
		}
	}
}

void EditScene::display() const
{
	if (_background && showBackground()) {
		_background->display(0.6f);
	}
	if (_flowMap && showFlowMap()) {
		_flowMap->drawMap();
	}
	if (_particles && !showParticles()) {
		_particles->draw(_screenSize, 0u);
	}
	if (_flowMap && showArrows()) {
		_flowMap->drawArrows(_screenSize);
	}
	if (showBrush()) {
		brush().display(_screenSize, iMousePos(), true);
	}
}

void EditScene::mouseMoved(glm::ivec2 const& movement)
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

void EditScene::doUpdate(float time, float dt)
{
	if (_particles && _background) {
		_particles->update(_screenSize, *_flowMap, *_background, time, dt);
	}
}

void EditScene::doHandleEvent(sf::Event const& event)
{
	switch (event.type) {
	case sf::Event::KeyPressed:
		if (event.key.code == sf::Keyboard::S) {
			glm::uvec2 bufferSize;
			std::vector<glm::vec2> buffer;
			_flowMap->getData(bufferSize, buffer);

			IO::saveFlowMap(_saveFilename, bufferSize, buffer);
		}
		else if (event.key.code == sf::Keyboard::R) {
			_flowMap->reset(1.f);
		}
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == sf::Mouse::Right) {
			glm::vec2 relativeMovement = glm::vec2(1);
			glm::vec2 relativeBrushSize = 2.f * glm::vec2(brush().radius()) / glm::vec2(_screenSize.x, _screenSize.y);
			_flowMap->changeLocally(mousePos(), relativeMovement, relativeBrushSize, true);
		}
			break;
	default:
		break;
	}
}