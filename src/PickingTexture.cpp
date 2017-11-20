#include "PickingTexture.hpp"

#include "GLHelper.hpp"

#include <iostream>
#include <array>


PickingTexture::PickingTexture(glm::uvec2 const& resolution):
	_bufferSize(resolution),
	_layerBuffer(resolution.x * resolution.y, PickingTexture::NO_LEVEL),
	_locked(false),
	_emptyVAO(0u),
	_FBO(0u),
	_layerTexture(0u),
	_buildingShader(new ShaderProgram())
{
	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Textures creation */
	{
		GLCHECK(glGenTextures(1, &_layerTexture));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, _layerTexture));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, resolution.x, resolution.y, 0, GL_RED, GL_UNSIGNED_BYTE, _layerBuffer.data()));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	/* FBO creation */
	{
		GLCHECK(glGenFramebuffers(1, &_FBO));
		GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _FBO));
		std::array<GLenum, 1> drawBuffers = { GL_COLOR_ATTACHMENT0};
		GLCHECK(glDrawBuffers(drawBuffers.size(), drawBuffers.data()));
	}

	/* Shader loading */
	if (!_buildingShader->loadFromFile("shaders/pickingTextureBuilder.vert", "shaders/pickingTextureBuilder.frag")) {
		std::cerr << "Error: unable to load shader pickingTextureBuilder" << std::endl;
	}
}

PickingTexture::~PickingTexture()
{
	freeOpenGLResources();
}

uint8_t PickingTexture::getLayer(glm::vec2 const& coords) const
{
	glm::uvec2 iCoords(coords.x * static_cast<float>(_bufferSize.x),
		coords.y * static_cast<float>(_bufferSize.y));

	if (coords.x >= _bufferSize.x || coords.y >= _bufferSize.y)
		return PickingTexture::NO_LEVEL;

	return _layerBuffer[iCoords.x + iCoords.y * _bufferSize.x];
}

void PickingTexture::addBackground(Background& background, uint8_t layer)
{
	if (_locked)
		return;

	if (!_buildingShader || !_buildingShader->isValid())
		return;

	/* FBO setup */
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _FBO));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _layerTexture, 0));
	GLCHECK(glViewport(0, 0, _bufferSize.x, _bufferSize.y));

	ShaderProgram::bind(*_buildingShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(*_buildingShader, "background", background.textureId());

	GLuint layerULoc = _buildingShader->getUniformLocation("layer");
	if (layerULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1ui(layerULoc, layer));
	}

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}

void PickingTexture::lock()
{
	GLCHECK(glBindTexture(GL_TEXTURE_2D, _layerTexture));
	GLCHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, _layerBuffer.data()));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, 0u));

	freeOpenGLResources();
}

void PickingTexture::freeOpenGLResources()
{
	_locked = true;

	_buildingShader.release();

	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));

	GLCHECK(glDeleteFramebuffers(1, &_FBO));

	GLCHECK(glDeleteTextures(1, &_layerTexture));
}