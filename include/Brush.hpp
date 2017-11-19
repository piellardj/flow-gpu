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
	Brush();
	virtual ~Brush();

	void display(glm::vec2 const& pos, glm::vec2 const& size) const;

private:
	GLuint _emptyVAO = 0u;
	mutable ShaderProgram _brushShader;
};

#endif // BRUSH_HPP_INCLUDED