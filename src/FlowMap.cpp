#include "FlowMap.hpp"


#include "GLHelper.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>


FlowMap::FlowMap(glm::uvec2 const& bufferSize, std::vector<glm::vec2> flowBuffer) :
	_bufferSize(bufferSize),
	_nbArrows(30u,30u),
	_emptyVAO(0u),
	_updateFBO(0u),
	_flowTexture({ 0u,0u }),
	_currBufferNo(0u),
	_arrowsVAO(0u),
	_arrowVBO(0u),
	_arrowsPosVBO(0u)
{
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
		unsigned int expectedSize = bufferSize.x * bufferSize.y;
		if (flowBuffer.size() != expectedSize) {
			std::cout << "FlowMap: provided buffer was not of the right size (expected " << expectedSize << ", received " << flowBuffer.size() << ")." << std::endl;
			flowBuffer.resize(bufferSize.x * bufferSize.y, glm::vec2(0.001f, 0.f));
		}

		GLCHECK(glGenTextures(_flowTexture.size(), _flowTexture.data()));
		for (GLuint& bufferId : _flowTexture) {
			GLCHECK(glBindTexture(GL_TEXTURE_2D, bufferId));
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _bufferSize.x, _bufferSize.y, 0, GL_RG, GL_FLOAT, flowBuffer.data()));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		}
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	{
		GLCHECK(glGenVertexArrays(1, &_arrowsVAO));
		GLCHECK(glBindVertexArray(_arrowsVAO));

		std::vector<glm::vec2> vert;
		vert.emplace_back(+0.f, -.2f);
		vert.emplace_back(+2.f, +0.f);
		vert.emplace_back(+0.f, +.2f);
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
	if (!_updateShader.loadFromFile("shaders/flowBufferUpdate.vert", "shaders/flowBufferUpdate.frag")) {
		std::cerr << "Error: unable to load shader flowBufferUpdate" << std::endl;
	}
	if (!_drawMapShader.loadFromFile("shaders/flowBufferDisplay.vert", "shaders/flowBufferDisplay.frag")) {
		std::cerr << "Error: unable to load shader flowBufferDisplay" << std::endl;
	}
	if (!_drawArrowsShader.loadFromFile("shaders/flowBufferDraw.vert", "shaders/flowBufferDraw.frag")) {
		std::cerr << "Error: unable to load shader flowBufferDraw" << std::endl;
	}
}

FlowMap::~FlowMap()
{
	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));
	GLCHECK((glDeleteFramebuffers(1, &_updateFBO)));
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

void FlowMap::addFlow(glm::vec2 const& pos, glm::vec2 const& flow, float brushSize)
{
	if (!_updateShader.isValid())
		return;

	switchBuffer();

	GLint previousFramebufferId = 0;
	GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebufferId));

	/* FBO setup */
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, currBufferId(), 0));
	GLCHECK(glViewport(0, 0, _bufferSize.x, _bufferSize.y));
	GLCHECK(glClear(GL_COLOR_BUFFER_BIT));

	ShaderProgram::bind(_updateShader);

	/* Uniforms setup */
	TextureBinder textureBinder;
	textureBinder.bindTexture(_updateShader, "previousBuffer", prevBufferId());

	GLuint flowULoc = _updateShader.getUniformLocation("flow");
	if (flowULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(flowULoc, flow.x, flow.y));
	}
	GLuint brushSizeULoc = _updateShader.getUniformLocation("brushSize");
	if (brushSizeULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(brushSizeULoc, brushSize, brushSize));
	}
	GLuint brushPosULoc = _updateShader.getUniformLocation("brushPos");
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

void FlowMap::drawMap() const
{
	if (!_drawMapShader.isValid())
		return;

	ShaderProgram::bind(_drawMapShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_drawMapShader, "flowBuffer", currBufferId());

	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
}

void FlowMap::drawArrows() const
{
	if (!_drawArrowsShader.isValid())
		return;

	ShaderProgram::bind(_drawArrowsShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_drawArrowsShader, "flowBuffer", currBufferId());

	GLuint sizeULoc = _drawArrowsShader.getUniformLocation("arrowSize");
	if (sizeULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(sizeULoc, .7f / static_cast<float>(_nbArrows.x)));
	}
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