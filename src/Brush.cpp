#include "Brush.hpp"


#include "GLHelper.hpp"

#include <iostream>


Brush::Brush()
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

void Brush::display(glm::vec2 const& pos, glm::vec2 const& size) const
{
	ShaderProgram::bind(_brushShader);

	GLuint posULoc = _brushShader.getUniformLocation("brushPos");
	if (posULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(posULoc, pos.x, pos.y));
	}
	GLuint sizeULoc = _brushShader.getUniformLocation("brushSize");
	if (sizeULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(sizeULoc, size.x, size.y));
	}

	GLCHECK(glEnable(GL_BLEND));
	GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}