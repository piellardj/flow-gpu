#ifndef BACKGROUND_HPP_INCLUDED
#define BACKGROUND_HPP_INCLUDED


#include <string>

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "ShaderProgram.hpp"
#include "NonCopyable.hpp"


/*! \class Background
 * \brief Stores and displays a 32bit 2D bit texture
 */
class Background: public NonCopyable
{
public:
	/*! Constructor
	 * If the parameters are invalid (size not matching, empty buffer...),
	 * a default 1x1 texture is used.
	 * \param buffer channels order: RGBA
	 */
	Background(glm::uvec2& bufferSize, std::vector<uint8_t> const& buffer);
	virtual ~Background();

	void display(float alpha=1.f) const;

	inline GLuint textureId() { return _backgroundTexture; }

private:
	GLuint _emptyVAO = 0u;
	GLuint _backgroundTexture = 0u;

	mutable ShaderProgram _backgroundShader;
};

#endif // BACKGROUND_HPP_INCLUDED