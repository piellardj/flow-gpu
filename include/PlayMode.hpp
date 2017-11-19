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
	struct LevelFilename
	{
		LevelFilename(std::string const& back, std::string const& flow) :
			backgroundFilename(back), flowFilename(flow)
		{}

		std::string backgroundFilename;
		std::string flowFilename;
	};

	PlayMode(std::vector<LevelFilename> const& lvlFilenames, sf::Window const& window);
	virtual ~PlayMode() {}

	virtual void update(float time);
	virtual void display() const;

private:
	virtual void mouseMoved(glm::vec2 const& movement);
	virtual void doHandleEvent(sf::Event const& event);

private:
	struct Level
	{
		std::unique_ptr<Background> background;
		std::unique_ptr<DensityMap> densityMap;
		std::unique_ptr<FlowMap> flowMap;
		std::unique_ptr<Particles> particles;
	};

	std::vector<Level> _levels;

	PickingTexture _picking;
	uint8_t _lastLevelPicked;
};

#endif // PLAY_MODE_HPP_INCLUDED