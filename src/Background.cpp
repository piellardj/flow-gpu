#include "Background.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"

#include <iostream>
#include <array>
#include <SFML/Graphics/Image.hpp>


Background::Background(glm::uvec2 const& bufferSize, std::vector<uint8_t> const& buffer) :
	_emptyVAO(0u),
	_backgroundTexture(0u)
{
	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Texture loading */
	{
		GLCHECK(glGenTextures(1, &_backgroundTexture));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, _backgroundTexture));
		if (buffer.size() != 4u * bufferSize.x * bufferSize.y) {
			std::cout << "Background: the provided buffer was not of the right size." << std::endl;
			std::array<uint8_t, 4> data = { 0, 255, 0, 255};
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
		}
		else {
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, bufferSize.x, bufferSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data()));
		}
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

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

void Background::display(float brightness) const
{
	ShaderProgram::bind(_backgroundShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_backgroundShader, "backgroundTexture", _backgroundTexture);

	GLuint brightnessULoc = _backgroundShader.getUniformLocation("brightness");
	if (brightnessULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(brightnessULoc, brightness));
	}

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glEnable(GL_BLEND));
	GLCHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}