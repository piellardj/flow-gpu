#include "Background.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"

#include <iostream>
#include <array>
#include <SFML/Graphics/Image.hpp>


Background::Background(glm::uvec2& bufferSize, std::vector<uint8_t> const& buffer) :
	_emptyVAO(0u),
	_backgroundTexture(0u)
{
	const glm::uvec2 defaultBufferSize(1u, 1u);
	const std::vector<uint8_t> defaultBuffer =  { 0u, 255u, 0u, 255u };

	uint8_t const* data = buffer.data();

	if (buffer.empty() || buffer.size() != 4u * bufferSize.x * bufferSize.y) {
		std::cerr << "Background: the provided buffer was invalid. The default background will be used instead." << std::endl;
		bufferSize = defaultBufferSize;
		data = defaultBuffer.data();
	}

	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Texture loading */
	GLCHECK(glGenTextures(1, &_backgroundTexture));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, _backgroundTexture));
	GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));

	/* Shaders loading */
	if (!_backgroundShader.loadFromFile("shaders/backgroundDisplay.vert", "shaders/backgroundDisplay.frag")) {
		std::cerr << "Error: unable to load shader backgroundDisplay" << std::endl;
	}
}

Background::~Background()
{
	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));
	GLCHECK((glDeleteTextures(1, &_backgroundTexture)));
}

void Background::display(float alpha) const
{
	if (!_backgroundShader.isValid())
		return;

	ShaderProgram::bind(_backgroundShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_backgroundShader, "backgroundTexture", _backgroundTexture);

	GLuint alphaULoc = _backgroundShader.getUniformLocation("alpha");
	if (alphaULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(alphaULoc, alpha));
	}

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glEnable(GL_BLEND));
	GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}