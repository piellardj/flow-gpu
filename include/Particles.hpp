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
/*! \class Particles
 * Class for managing and displaying particles.
 * The particles move according to a vector field (FlowMap).
 * The particles have a limited lifetime, after which they are reset to the initial position.
 */
class Particles: public NonCopyable
{
public:
	/*! \param initPos : Normalized initial positions.
	 *                   If the vector's size is not a square number, the last positions will not be used.
	 */
	Particles(std::vector<glm::vec2> const& initPos);
	virtual ~Particles();

	/*! \param time : elapsed time since the start of the simulation
	 * \param dt : elapsed time since last update */
	void update(glm::uvec2 const& screenSize, FlowMap& flowMap, Background& background, float time, float dt);
	void draw(glm::uvec2 const& screenSize, unsigned int layer) const;

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