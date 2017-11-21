#ifndef EDIT_MODE_HPP_INCLUDED
#define EDIT_MODE_HPP_INCLUDED


#include "Scene.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"
#include "Particles.hpp"

#include <string>
#include <memory>


class EditScene: public Scene
{
public:
	struct Description
	{
		std::string saveFilename;
		std::string backgroundFilename;
		std::string flowmapFilename;
		glm::uvec2 defaultResolution; //to use if flowmapFilename couldn't be loaded
	};

public:
	EditScene(Description const& description, sf::Window const& window);
	virtual ~EditScene() {}
	
	virtual void display() const;

private:
	virtual void mouseMoved(glm::ivec2 const& movement);
	virtual void doUpdate(float time, float dt);
	virtual void doHandleEvent(sf::Event const& event);

private:
	const std::string _saveFilename;

	std::unique_ptr<FlowMap> _flowMap;
	std::unique_ptr<Background> _background;
	std::unique_ptr<Particles> _particles;
};

#endif // EDIT_MODE_HPP_INCLUDED