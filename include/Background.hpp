#ifndef BACKGROUND_HPP_INCLUDED
#define BACKGROUND_HPP_INCLUDED


#include <string>

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "ShaderProgram.hpp"
#include "NonCopyable.hpp"


class Background: public NonCopyable
{
public:
	Background(glm::uvec2 const& bufferSize, std::vector<uint8_t> const& buffer);
	virtual ~Background();

	void display(float brightness=1.f) const;

	inline GLuint textureId() { return _backgroundTexture; }

private:
	GLuint _emptyVAO = 0u;
	GLuint _backgroundTexture = 0u;

	mutable ShaderProgram _backgroundShader;
};

#endif // BACKGROUND_HPP_INCLUDED