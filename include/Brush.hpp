#ifndef BRUSH_HPP_INCLUDED
#define BRUSH_HPP_INCLUDED


#include "NonCopyable.hpp"

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "ShaderProgram.hpp"


class Brush: public NonCopyable
{
public:
	/*! Constructor
	 * The radius is in pixels. */
	Brush(float minRadius, float maxRadius, unsigned int nbSteps=10u);
	virtual ~Brush();

	/*! \return radius in pixels */
	inline float radius() const { return _radius; }

	void changeRadius(float nbSteps);

	void display(glm::uvec2 const& screenSize, glm::ivec2 const& pos) const;

private:
	const float _minRadius, _maxRadius;
	const float _step;
	float _radius;

	GLuint _emptyVAO = 0u;
	mutable ShaderProgram _brushShader;
};

#endif // BRUSH_HPP_INCLUDED