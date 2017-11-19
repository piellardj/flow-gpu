#ifndef EDIT_MODE_HPP_INCLUDED
#define EDIT_MODE_HPP_INCLUDED


#include "Mode.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"
#include "Brush.hpp"

#include <string>
#include <memory>


class EditMode: public Mode
{
public:
	EditMode(std::string const& saveFilename, sf::Window const& window, std::string const& flowmapFilename="");
	virtual ~EditMode() {}

	void setBackground(std::string const& filename);

	virtual void update(float time);
	virtual void display() const;

private:
	virtual void mouseMoved(glm::vec2 const& movement);
	virtual void doHandleEvent(sf::Event const& event);

private:
	const std::string _saveFilename;

	std::unique_ptr<FlowMap> _flowMap;
	std::unique_ptr<Background> _background;
	Brush _brush;

	const float MIN_BRUSH_SIZE;
	const float MAX_BRUSH_SIZE;
	float _brushSize;
};

#endif // EDIT_MODE_HPP_INCLUDED