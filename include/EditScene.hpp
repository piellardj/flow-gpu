#ifndef EDIT_MODE_HPP_INCLUDED
#define EDIT_MODE_HPP_INCLUDED


#include "Scene.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"

#include <string>
#include <memory>


class EditScene: public Scene
{
public:
	EditScene(std::string const& saveFilename, sf::Window const& window, std::string const& flowmapFilename="");
	virtual ~EditScene() {}

	void setBackground(std::string const& filename);

	virtual void display() const;

private:
	virtual void mouseMoved(glm::ivec2 const& movement);
	virtual void doUpdate(float time, float dt);
	virtual void doHandleEvent(sf::Event const& event);

private:
	const std::string _saveFilename;

	std::unique_ptr<FlowMap> _flowMap;
	std::unique_ptr<Background> _background;
};

#endif // EDIT_MODE_HPP_INCLUDED