#ifndef PLAY_MODE_HPP_INCLUDED
#define PLAY_MODE_HPP_INCLUDED


#include "Mode.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"
#include "Particles.hpp"
#include "DensityMap.hpp"
#include "PickingTexture.hpp"

#include <SFML/System/Clock.hpp>
#include <memory>


class PlayMode : public Mode
{
public:
	struct LayerFilename
	{
		LayerFilename(std::string const& back, std::string const& flow) :
			backgroundFilename(back), flowFilename(flow)
		{}

		std::string backgroundFilename;
		std::string flowFilename;
	};

	PlayMode(std::vector<LayerFilename> const& lvlFilenames, sf::Window const& window);
	virtual ~PlayMode() {}

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