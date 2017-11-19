#ifndef PLAY_MODE_HPP_INCLUDED
#define PLAY_MODE_HPP_INCLUDED


#include "Mode.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"
#include "Particles.hpp"
#include "DensityMap.hpp"

#include <SFML/System/Clock.hpp>
#include <memory>


class PlayMode : public Mode
{
public:
	PlayMode(std::string const& backgroundFilename, std::string const& flowFilename, sf::Window const& window);
	virtual ~PlayMode() {}

	virtual void update(float time);
	virtual void display() const;

private:
	virtual void mouseMoved(glm::vec2 const& movement);
	virtual void doHandleEvent(sf::Event const& event);

private:
	const std::string _flowFilename;

	std::unique_ptr<Background> _background;
	std::unique_ptr<DensityMap> _densityMap;
	std::unique_ptr<FlowMap> _flowMap;
	std::unique_ptr<Particles> _particles;
};

#endif // PLAY_MODE_HPP_INCLUDED