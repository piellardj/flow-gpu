#include "FlowMap.hpp"


#include "GLHelper.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>


FlowMap::FlowMap(glm::uvec2 const& bufferSize, std::vector<glm::vec2> const& flowBuffer) :
	_bufferSize(bufferSize),
	_nbArrows(30u,30u),
	_emptyVAO(0u),
	_updateFBO(0u),
	_initTexture(0u),
	_flowTexture({ 0u,0u }),
	_currBufferNo(0u),
	_arrowsVAO(0u),
	_arrowVBO(0u),
	_arrowsPosVBO(0u)
{
	const glm::uvec2 defaultBufferSize(32u, 32u);
	const std::vector<glm::vec2> defaultBuffer(defaultBufferSize.x * defaultBufferSize.y, glm::vec2(0.f, 0.f));

	std::vector<glm::vec2> const* actualBuffer = &flowBuffer;
	if (flowBuffer.empty() || flowBuffer.size() != bufferSize.x * bufferSize.y) {
		_bufferSize = defaultBufferSize;
		actualBuffer = &defaultBuffer;
	}

	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Update framebuffer creation */
	{
		GLint previousFBO = 0;
		GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO));

		GLCHECK(glGenFramebuffers(1, &_updateFBO));
		GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
		GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
		GLCHECK(glDrawBuffers(1, drawBuffers));

		GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, previousFBO));
	}

	/* Buffers creation */
	{
		std::array<GLuint*, 3> textures = { &_initTexture, &_flowTexture[0], &_flowTexture[1] };
		for (GLuint* texIdPtr : textures) {
			GLCHECK(glGenTextures(1, texIdPtr));
			GLCHECK(glBindTexture(GL_TEXTURE_2D, *texIdPtr));
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _bufferSize.x, _bufferSize.y, 0, GL_RG, GL_FLOAT, actualBuffer->data()));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		}
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	/* Arrows VBO and VAO creation */
	{
		GLCHECK(glGenVertexArrays(1, &_arrowsVAO));
		GLCHECK(glBindVertexArray(_arrowsVAO));

		std::vector<glm::vec2> vert = { {0,-.5}, {1,0}, {0,+.5} };
		GLCHECK(glGenBuffers(1, &_arrowVBO));
		GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _arrowVBO));
		GLCHECK(glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(glm::vec2), vert.data(), GL_STATIC_DRAW));
		const GLuint vertALoc = 0u;
		GLCHECK(glEnableVertexAttribArray(vertALoc));
		GLCHECK(glVertexAttribPointer(vertALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
		GLCHECK(glVertexAttribDivisor(vertALoc, 0)); //change pos for each instance

		glm::vec2 spacing = glm::vec2(2.f) / glm::vec2(_nbArrows.x + 1, _nbArrows.y + 1);
		std::vector<glm::vec2> pos;
		for (unsigned int iX = 0u; iX < _nbArrows.x; ++iX) {
			for (unsigned int iY = 0u; iY < _nbArrows.y; ++iY) {
				pos.emplace_back(glm::vec2(-1.f,-1.f) + spacing * glm::vec2(iX+1, iY+1));
			}
		}
		GLCHECK(glGenBuffers(1, &_arrowsPosVBO));
		GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _arrowsPosVBO));
		GLCHECK(glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2), pos.data(), GL_STATIC_DRAW));
		const GLuint posALoc = 1u;
		GLCHECK(glEnableVertexAttribArray(posALoc));
		GLCHECK(glVertexAttribPointer(posALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
		GLCHECK(glVertexAttribDivisor(posALoc, 1)); //change pos for each instance

		GLCHECK(glBindVertexArray(0u));
		GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	/* Shaders loading */
	if (!_drawMapShader.loadFromFile("shaders/flowBufferDrawMap.vert", "shaders/flowBufferDrawMap.frag")) {
		std::cerr << "Error: unable to load shader flowBufferDrawMap" << std::endl;
	}
	if (!_drawArrowsShader.loadFromFile("shaders/flowBufferDrawArrows.vert", "shaders/flowBufferDrawArrows.frag")) {
		std::cerr << "Error: unable to load shader flowBufferDrawArrows" << std::endl;
	}
	if (!_changeShader.loadFromFile("shaders/flowBufferChange.vert", "shaders/flowBufferChange.frag")) {
		std::cerr << "Error: unable to load shader flowBufferUpdate" << std::endl;
	}
	if (!_resetShader.loadFromFile("shaders/flowBufferReset.vert", "shaders/flowBufferReset.frag")) {
		std::cerr << "Error: unable to load shader flowBufferDraw" << std::endl;
	}
}

FlowMap::~FlowMap()
{
	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));
	GLCHECK((glDeleteFramebuffers(1, &_updateFBO)));
	GLCHECK((glDeleteTextures(1, &_initTexture)));
	GLCHECK((glDeleteTextures(_flowTexture.size(), _flowTexture.data())));

	GLCHECK(glDeleteVertexArrays(1, &_arrowsVAO));
	GLCHECK(glDeleteBuffers(1, &_arrowVBO));
	GLCHECK(glDeleteBuffers(1, &_arrowsPosVBO));
}

void FlowMap::getData(glm::uvec2& bufferSize, std::vector<glm::vec2>& flowBuffer) const
{
	bufferSize = _bufferSize;
	flowBuffer.resize(_bufferSize.x * _bufferSize.y);

	GLCHECK(glBindTexture(GL_TEXTURE_2D, currBufferId()));
	GLCHECK(glGetTexImage(GL_TEXTURE_2D, 0, GL_RG, GL_FLOAT, flowBuffer.data()));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, 0u));
}

void FlowMap::changeLocally(glm::vec2 const& pos, glm::vec2 const& movement, glm::vec2 const& brushSize, bool setNull)
{
	if (!_changeShader.isValid())
		return;

	switchBuffer();

	GLint previousFramebufferId = 0;
	GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebufferId));

	/* FBO setup */
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currBufferId(), 0));
	GLCHECK(glViewport(0, 0, _bufferSize.x, _bufferSize.y));
	GLCHECK(glClear(GL_COLOR_BUFFER_BIT));

	ShaderProgram::bind(_changeShader);

	/* Uniforms setup */
	TextureBinder textureBinder;
	textureBinder.bindTexture(_changeShader, "previousBuffer", prevBufferId());

	GLuint nullULoc = _changeShader.getUniformLocation("setNull");
	if (nullULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1ui(nullULoc, setNull));
	}
	GLuint movementULoc = _changeShader.getUniformLocation("movement");
	if (movementULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(movementULoc, movement.x, movement.y));
	}
	GLuint brushSizeULoc = _changeShader.getUniformLocation("brushSize");
	if (brushSizeULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(brushSizeULoc, brushSize.x, brushSize.y));
	}
	GLuint brushPosULoc = _changeShader.getUniformLocation("brushPos");
	if (brushPosULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(brushPosULoc, pos.x, pos.y));
	}

	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebufferId));
}

void FlowMap::reset(float factor)
{
	if (!_resetShader.isValid())
		return;

	switchBuffer();

	GLint previousFramebufferId = 0;
	GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebufferId));

	/* FBO setup */
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currBufferId(), 0));
	GLCHECK(glViewport(0, 0, _bufferSize.x, _bufferSize.y));
	GLCHECK(glClear(GL_COLOR_BUFFER_BIT));

	ShaderProgram::bind(_resetShader);

	/* Uniforms setup */
	TextureBinder textureBinder;
	textureBinder.bindTexture(_resetShader, "previousBuffer", prevBufferId());
	textureBinder.bindTexture(_resetShader, "initBuffer", _initTexture);

	GLuint factorULoc = _resetShader.getUniformLocation("factor");
	if (factorULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(factorULoc, factor));
	}

	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebufferId));
}

void FlowMap::drawMap() const
{
	if (!_drawMapShader.isValid())
		return;

	ShaderProgram::bind(_drawMapShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_drawMapShader, "flowBuffer", currBufferId());

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}

void FlowMap::drawArrows(glm::ivec2 const& screenSize) const
{
	if (!_drawArrowsShader.isValid())
		return;

	ShaderProgram::bind(_drawArrowsShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_drawArrowsShader, "flowBuffer", currBufferId());

	GLuint screenULoc = _drawArrowsShader.getUniformLocation("screenSize");
	if (screenULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(screenULoc, screenSize.x, screenSize.y));
	}
	GLuint sizeULoc = _drawArrowsShader.getUniformLocation("arrowSize");
	if (sizeULoc != ShaderProgram::nullLocation) {
		glm::vec2 spacing = glm::vec2(screenSize.x, screenSize.y) / glm::vec2(_nbArrows.x, _nbArrows.y);
		float resize = std::min(spacing.x, spacing.y);
		glm::vec2 arrowSize = glm::vec2(1.2, .5) * resize;
		GLCHECK(glUniform2f(sizeULoc, arrowSize.x, arrowSize.y));
	}
	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_arrowsVAO));
	GLCHECK(glDrawArraysInstanced(GL_TRIANGLES, 0, 3, _nbArrows.x*_nbArrows.y));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}

void FlowMap::switchBuffer()
{
	_currBufferNo = (_currBufferNo + 1u) % 2u;
}

GLuint FlowMap::currBufferId() const
{
	return _flowTexture[_currBufferNo];
}

GLuint FlowMap::prevBufferId() const
{
	unsigned int prevBufferNo = (_currBufferNo + 1u) % 2u;
	return _flowTexture[prevBufferNo];
}