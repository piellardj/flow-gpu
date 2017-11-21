#ifndef PLAY_MODE_HPP_INCLUDED
#define PLAY_MODE_HPP_INCLUDED


#include "Scene.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"
#include "Particles.hpp"
#include "DensityMap.hpp"
#include "PickingTexture.hpp"

#include <SFML/System/Clock.hpp>
#include <memory>


class PlayScene : public Scene
{
public:
	struct LayerDescription
	{
		/*!
		 * \param sqNb Square root of the wanted amount of particles for this layer */
		LayerDescription(std::string const& back, std::string const& flow, unsigned int sqNb) :
			backgroundFilename(back), flowFilename(flow), sqNbParticles(sqNb)
		{}

		std::string backgroundFilename;
		std::string flowFilename;
		unsigned int sqNbParticles;
	};
	typedef std::vector<LayerDescription> Description;

public:
	PlayScene(Description const& description, sf::Window const& window);
	virtual ~PlayScene() {}
	
	virtual void display() const;

private:
	virtual void mouseMoved(glm::ivec2 const& movement);
	virtual void doUpdate(float time, float dt);
	virtual void doHandleEvent(sf::Event const& event);

private:
	struct Layer
	{
		std::unique_ptr<Background> background;
		std::unique_ptr<DensityMap> densityMap;
		std::unique_ptr<FlowMap> flowMap;
		std::unique_ptr<Particles> particles;
	};

	std::vector<Layer> _layers;

	PickingTexture _picking;
	uint8_t _lastLayerPicked;
};

#endif // PLAY_MODE_HPP_INCLUDED