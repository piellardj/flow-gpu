#ifndef PARTICLES_HPP_INCLUDED
#define PARTICLES_HPP_INCLUDED

#include "NonCopyable.hpp"

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <array>

#include "ShaderProgram.hpp"


class FlowMap;
class Background;
class Particles: public NonCopyable
{
public:
	Particles(std::vector<glm::vec2> const& initPos);
	virtual ~Particles();

	void update(FlowMap& flowMap, Background& background, float time, float dt);
	void draw(unsigned int layer) const;

	unsigned int nbParticles() const;

private:
	void switchBuffer();
	unsigned int currBufferNo() const;
	unsigned int prevBufferNo() const;

private:
	const glm::uvec2 _bufferSize;
	float _lastUpdate;

	GLuint _emptyVAO = 0u;
	GLuint _updateFBO = 0u;
	std::array<GLuint, 2> _posTexture; //RG32F, R->pos.x, G->pos.y
	GLuint _initPosTexture; //RG16F, R->pos.x, G->pos.y
	std::array<GLuint, 2> _birthdateTexture; //R16UI
	std::array<GLuint, 2> _looksTexture; //RGBA8, RGB->colour, A->orientation in [0,1]
	unsigned int _currBufferNo; // in 0.._buffersId.size()-1

	GLuint _drawVAO = 0u;
	GLuint _strokeCornersVBO = 0u;
	GLuint _texelsVBO = 0u;
	GLuint _strokeTexture = 0u;

	mutable ShaderProgram _drawShader;
	mutable ShaderProgram _updateShader;
};

#endif // PARTICLES_HPP_INCLUDED