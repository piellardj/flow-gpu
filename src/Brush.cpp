#include "Brush.hpp"


#include "GLHelper.hpp"

#include <iostream>
#include <algorithm>


Brush::Brush(float minRadius, float maxRadius, unsigned int nbSteps) :
	_minRadius(std::max(0.f, minRadius)),
	_maxRadius(std::max(0.f, maxRadius)),
	_step((_maxRadius - _minRadius) / static_cast<float>(nbSteps)),
	_radius(0.5f * (_minRadius + _maxRadius))
{
	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Shaders loading */
	if (!_brushShader.loadFromFile("shaders/brushDisplay.vert", "shaders/brushDisplay.frag")) {
		std::cerr << "Error: unable to load shader brushDisplay" << std::endl;
	}
}

Brush::~Brush()
{
	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));
}

void Brush::changeRadius(float nbSteps)
{
	_radius += nbSteps * _step;
	_radius = std::max(_minRadius, std::min(_maxRadius, _radius));
}

void Brush::display(glm::uvec2 const& screenSize, glm::ivec2 const& pos, bool onlyRing) const
{
	if (!_brushShader.isValid())
		return;

	ShaderProgram::bind(_brushShader);

	GLuint ringULoc = _brushShader.getUniformLocation("onlyRing");
	if (ringULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1ui(ringULoc, onlyRing));
	}
	GLuint screenULoc = _brushShader.getUniformLocation("screenSize");
	if (screenULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(screenULoc, screenSize.x, screenSize.y));
	}
	GLuint posULoc = _brushShader.getUniformLocation("brushPos");
	if (posULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2i(posULoc, pos.x, pos.y));
	}
	GLuint radiusULoc = _brushShader.getUniformLocation("brushRadius");
	if (radiusULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(radiusULoc, _radius));
	}

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glEnable(GL_BLEND));
	GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}