#include "PlayScene.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"
#include "glm.hpp"

#include <iostream>
#include <SFML/Graphics/Image.hpp>


PlayScene::PlayScene(Description const& description, sf::Window const& window) :
	Scene(window),
	_picking(glm::uvec2(window.getSize().x, window.getSize().y))
{
	for (unsigned int iL = 0u; iL < description.size(); ++iL) {
		_layers.push_back(Layer());

		LayerDescription const& lvlDescription = description[iL];
		Layer& lvl = _layers.back();

		std::cout << "=== Loading layer " << iL << " ===\n";
		
		glm::uvec2 backgroundSize;
		std::vector<uint8_t> backgroundBuffer;
		if (IO::load32bitImage(lvlDescription.backgroundFilename, backgroundSize, backgroundBuffer)) {
			std::cout << "Loaded background '" << lvlDescription.backgroundFilename << "'\n";
		}
		else {
			backgroundSize = glm::uvec2(1u, 1u);
			backgroundBuffer.resize(4u, 128u);
		}

		glm::uvec2 flowSize;
		std::vector<glm::vec2> flowBuffer;
		if (IO::loadFlowMap(lvlDescription.flowFilename, flowSize, flowBuffer)) {
			std::cout << "Loaded flow map '" << lvlDescription.flowFilename << "'\n";
		}
		else {
			flowSize = glm::uvec2(32u, 32u);
			flowBuffer.resize(flowSize.x * flowSize.y, glm::uvec2(0.f, 0.f));
		}

		/* Background loading */
		{
			lvl.background.reset(new Background(backgroundSize, backgroundBuffer));
			_picking.addBackground(*lvl.background, iL);

			std::vector<uint8_t> density(backgroundSize.x * backgroundSize.y);
			for (unsigned int i = 0u; i < density.size(); ++i) {
				density[i] = backgroundBuffer[4u * i + 3];
			}

			lvl.densityMap.reset(new DensityMap(backgroundSize, density));
		}

		/* Flow map loading */
		lvl.flowMap.reset(new FlowMap(flowSize, flowBuffer));

		unsigned int nbParticles = lvlDescription.sqNbParticles * lvlDescription.sqNbParticles;
		std::vector<glm::vec2> initPos = lvl.densityMap->sample(std::max(0u, nbParticles));
		lvl.particles.reset(new Particles(initPos));
		std::cout << "Generated " << lvl.particles->nbParticles() << " particles\n" << std::endl;
	}

	_picking.lock();
}

void PlayScene::display() const
{
	for (Layer const& lvl : _layers) {
		if (lvl.background && showBackground()) {
			lvl.background->display();
		}
	}

	if (showParticles()) {
		for (unsigned int iL = 0u; iL < _layers.size(); ++iL) {
			_layers[iL].particles->draw(_screenSize, iL);
		}
	}

	if (showBrush()) {
		brush().display(_screenSize, iMousePos(), false);
	}
}

void PlayScene::mouseMoved(glm::ivec2 const& movement)
{
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		glm::vec2 relativeMovement = glm::vec2(movement.x, movement.y) / glm::vec2(_screenSize.x, _screenSize.y);
		glm::vec2 relativeBrushSize = 2.f * glm::vec2(brush().radius()) / glm::vec2(_screenSize.x, _screenSize.y);

		if (_lastLayerPicked < _layers.size()) {
			_layers[_lastLayerPicked].flowMap->changeLocally(mousePos(), relativeMovement, relativeBrushSize);
		}
	}
}

void PlayScene::doUpdate(float time, float dt)
{
	for (Layer const& lvl : _layers) {
		lvl.particles->update(_screenSize, *lvl.flowMap, *lvl.background, time, dt);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		dt = std::max(0.01f, std::min(0.5f, dt));

		for (Layer const& lvl : _layers) {
			lvl.flowMap->reset(2.f * dt);// dt * 20.f);
		}
	}
}

void PlayScene::doHandleEvent(sf::Event const& event)
{
	switch (event.type) {
	case sf::Event::MouseButtonPressed:
		if (event.mouseButton.button == sf::Mouse::Left) {
			_lastLayerPicked = _picking.getLayer(mousePos());
		}
		break;
	default:
		break;
	}
}