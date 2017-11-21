#include "Particles.hpp"


#include "GLHelper.hpp"
#include "IO.hpp"
#include "FlowMap.hpp"
#include "Background.hpp"

#include <SFML/Graphics/Image.hpp>

#include <iostream>

#include <cstdlib>
#include <ctime>


static float random(float from = 0.f, float to = 1.f)
{
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		std::srand(static_cast<unsigned int>(std::time(NULL)));
	}

	float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	return r * (to - from) + from;
}

Particles::Particles(std::vector<glm::vec2> const& initPos) :
	_bufferSize(std::max(1u, (unsigned int)(std::sqrt(initPos.size())))),
	_lastUpdate(0.f),
	_emptyVAO(0u),
	_updateFBO(0u),
	_posTexture({0u, 0u}),
	_initPosTexture(0u),
	_birthdateTexture({0u, 0u}),
	_looksTexture({0u,0u}),
	_currBufferNo(0u),
	_drawVAO(0u),
	_texelsVBO(0u),
	_strokeTexture(0u)
{
	const std::vector<glm::vec2> defaultInitPos(1, glm::vec2(0.5f,0.5f));
	std::vector<glm::vec2> const* initPosPtr = &initPos;
	if (initPos.empty()) {
		initPosPtr = &defaultInitPos;
	}

	/* VAO creation */
	GLCHECK(glGenVertexArrays(1, &_emptyVAO));

	/* Update framebuffer creation */
	{
		GLint previousFBO = 0;
		GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO));

		GLCHECK(glGenFramebuffers(1, &_updateFBO));
		GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
		std::array<GLenum, 3> drawBuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		GLCHECK(glDrawBuffers(drawBuffers.size(), drawBuffers.data()));

		GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, previousFBO));
	}

	/* Buffers allocation */
	{
		GLCHECK(glGenTextures(_posTexture.size(), _posTexture.data()));
		for (GLuint& bufferId : _posTexture) {
			GLCHECK(glBindTexture(GL_TEXTURE_2D, bufferId));
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, _bufferSize.x, _bufferSize.y, 0, GL_RG, GL_FLOAT, initPos.data()));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		}

		GLCHECK(glGenTextures(1, &_initPosTexture));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, _initPosTexture));
		GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, _bufferSize.x, _bufferSize.y, 0, GL_RG, GL_FLOAT, initPos.data()));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	}
	{
		std::vector<GLfloat> birthdate(_bufferSize.x*_bufferSize.y);
		for (float& t : birthdate) {
			t = random(0.f, 5.f);
		}
		GLCHECK(glGenTextures(_birthdateTexture.size(), _birthdateTexture.data()));
		for (GLuint& bufferId : _birthdateTexture) {
			GLCHECK(glBindTexture(GL_TEXTURE_2D, bufferId));
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _bufferSize.x, _bufferSize.y, 0, GL_RED, GL_FLOAT, birthdate.data()));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		}
	}
	{
		std::vector<glm::vec4> colorOrientation(_bufferSize.x*_bufferSize.y, glm::vec4(1.f, 0.f, 0.f, 0.f));
		for (glm::vec4& color : colorOrientation) {
			color.g = random(0.f, 1.f);
		}
		GLCHECK(glGenTextures(_looksTexture.size(), _looksTexture.data()));
		for (GLuint& bufferId : _looksTexture) {
			GLCHECK(glBindTexture(GL_TEXTURE_2D, bufferId));
			GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _bufferSize.x, _bufferSize.y, 0, GL_RGBA, GL_FLOAT, colorOrientation.data()));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		}

		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	/* Draw VBO and VAO */
	{
		GLCHECK(glGenVertexArrays(1, &_drawVAO));
		GLCHECK(glBindVertexArray(_drawVAO));

		{
			std::vector<glm::ivec2> texels;
			for (unsigned int iX = 0u; iX < _bufferSize.x; ++iX) {
				for (unsigned int iY = 0u; iY < _bufferSize.y; ++iY) {
					texels.emplace_back(iX, iY);
				}
			}
			GLCHECK(glGenBuffers(1, &_texelsVBO));
			GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _texelsVBO));
			GLCHECK(glBufferData(GL_ARRAY_BUFFER, texels.size() * sizeof(glm::ivec2), texels.data(), GL_STATIC_DRAW));

			const GLuint texelALoc = 0u;
			GLCHECK(glEnableVertexAttribArray(texelALoc));
			GLCHECK(glVertexAttribIPointer(texelALoc, 2, GL_INT, 0, (void*)0));
			GLCHECK(glVertexAttribDivisor(texelALoc, 1)); //change texel for each instance
		}

		{
			std::vector<glm::vec2> corners;
			corners.emplace_back(-1.f, -1.f);
			corners.emplace_back(+1.f, -1.f);
			corners.emplace_back(-1.f, +1.f);
			corners.emplace_back(+1.f, +1.f);
			GLCHECK(glGenBuffers(1, &_strokeCornersVBO));
			GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, _strokeCornersVBO));
			GLCHECK(glBufferData(GL_ARRAY_BUFFER, corners.size() * sizeof(glm::vec2), corners.data(), GL_STATIC_DRAW));

			const GLuint cornersALoc = 1u;
			GLCHECK(glEnableVertexAttribArray(cornersALoc));
			GLCHECK(glVertexAttribPointer(cornersALoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
			GLCHECK(glVertexAttribDivisor(cornersALoc, 0)); //reuse the same 4 corners for each instance
		}

		GLCHECK(glBindVertexArray(0));
		GLCHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	/* Texture loading */
	{
		glm::uvec2 bufferSize;
		std::vector<uint8_t> buffer;

		if (!IO::loadAlphaMask("rc/stroke.bmp", bufferSize, buffer)) {
			bufferSize = glm::uvec2(1u, 1u);
			buffer.resize(1u, 255u);
		}

		GLCHECK(glGenTextures(1, &_strokeTexture));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, _strokeTexture));
		GLCHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, bufferSize.x, bufferSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, buffer.data()));
		const GLfloat borderColor[] = { 0.f, 0.f, 0.f, 0.f };
		GLCHECK(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));
		//GLCHECK(glGenerateMipmap(GL_TEXTURE_2D));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));// _MIPMAP_LINEAR));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
		GLCHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0));
	}

	/* Shaders loading */
	if (!_updateShader.loadFromFile("shaders/particlesUpdate.vert", "shaders/particlesUpdate.frag")) {
		std::cerr << "Error: unable to load shader particlesUpdate" << std::endl;
	}
	if (!_drawShader.loadFromFile("shaders/particlesDraw.vert", "shaders/particlesDraw.frag")) {
		std::cerr << "Error: unable to load shader particlesDraw" << std::endl;
	}
}

Particles::~Particles()
{
	GLCHECK(glDeleteVertexArrays(1, &_emptyVAO));

	GLCHECK(glDeleteFramebuffers(1, &_updateFBO));

	GLCHECK(glDeleteTextures(_posTexture.size(), _posTexture.data()));
	GLCHECK(glDeleteTextures(1, &_initPosTexture));
	GLCHECK(glDeleteTextures(_birthdateTexture.size(), _birthdateTexture.data()));
	GLCHECK(glDeleteTextures(_looksTexture.size(), _looksTexture.data()));
	GLCHECK(glDeleteTextures(1, &_strokeTexture));

	GLCHECK(glDeleteBuffers(1, &_strokeCornersVBO));
}

void Particles::update(glm::uvec2 const& screenSize, FlowMap& flowMap, Background& background, float time, float dt)
{
	_lastUpdate = time;

	if (!_updateShader.isValid())
		return;

	switchBuffer();

	GLint previousFBO = 0;
	GLCHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFBO));

	/* FBO setup */
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, _updateFBO));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _posTexture[currBufferNo()], 0));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _looksTexture[currBufferNo()], 0));
	GLCHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, _birthdateTexture[currBufferNo()], 0));
	GLCHECK(glViewport(0, 0, _bufferSize.x, _bufferSize.y));
	GLCHECK(glClear(GL_COLOR_BUFFER_BIT));

	ShaderProgram::bind(_updateShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_updateShader, "flowMap", flowMap.getTextureId());
	textureBinder.bindTexture(_updateShader, "previousPosBuffer", _posTexture[prevBufferNo()]);
	textureBinder.bindTexture(_updateShader, "previousLooksBuffer", _looksTexture[prevBufferNo()]);
	textureBinder.bindTexture(_updateShader, "initPosBuffer", _initPosTexture);
	textureBinder.bindTexture(_updateShader, "birthdateBuffer", _birthdateTexture[prevBufferNo()]);
	textureBinder.bindTexture(_updateShader, "colorBuffer", background.textureId());

	GLuint screenULoc = _updateShader.getUniformLocation("screenSize");
	if (screenULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(screenULoc, screenSize.x, screenSize.y));
	}
	GLuint dtULoc = _updateShader.getUniformLocation("dt");
	if (dtULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(dtULoc, dt));
	}

	GLuint clockULoc = _updateShader.getUniformLocation("clock");
	if (clockULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(clockULoc, time));
	}

	GLCHECK(glDisable(GL_DEPTH_TEST));
	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glBindVertexArray(_emptyVAO));
	GLCHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	GLCHECK(glBindVertexArray(0));

	ShaderProgram::unbind();
	GLCHECK(glBindFramebuffer(GL_FRAMEBUFFER, previousFBO));
}

void Particles::draw(glm::uvec2 const& screenSize, unsigned int layer) const
{
	if (!_drawShader.isValid())
		return;

	ShaderProgram::bind(_drawShader);

	TextureBinder textureBinder;
	textureBinder.bindTexture(_drawShader, "posBuffer", _posTexture[currBufferNo()]);
	textureBinder.bindTexture(_drawShader, "looksBuffer", _looksTexture[currBufferNo()]);
	textureBinder.bindTexture(_drawShader, "birthdateBuffer", _birthdateTexture[currBufferNo()]);
	textureBinder.bindTexture(_drawShader, "strokeTexture", _strokeTexture);

	GLuint screenULoc = _drawShader.getUniformLocation("screenSize");
	if (screenULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform2f(screenULoc, screenSize.x, screenSize.y));
	}
	GLuint clockULoc = _drawShader.getUniformLocation("clock");
	if (clockULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1f(clockULoc, _lastUpdate));
	}

	GLuint lvlULoc = _drawShader.getUniformLocation("layer");
	if (lvlULoc != ShaderProgram::nullLocation) {
		GLCHECK(glUniform1ui(lvlULoc, layer));
	}

	/* Actual drawing */
	GLCHECK(glDisable(GL_BLEND));
	GLCHECK(glEnable(GL_DEPTH_TEST));
	GLCHECK(glBindVertexArray(_drawVAO));
	GLCHECK(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, nbParticles()));
	GLCHECK(glBindVertexArray(0));

	GLCHECK(glBindVertexArray(0));
	ShaderProgram::unbind();
}

unsigned int Particles::nbParticles() const
{
	return _bufferSize.x * _bufferSize.y;
}

void Particles::switchBuffer()
{
	_currBufferNo = (_currBufferNo + 1u) % 2u;
}

unsigned int Particles::currBufferNo() const
{
	return _currBufferNo;
}

unsigned int Particles::prevBufferNo() const
{
	return (_currBufferNo + 1u) % 2u;
}