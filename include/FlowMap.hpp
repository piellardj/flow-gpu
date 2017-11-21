#ifndef FLOW_MAP_HPP_INCLUDED
#define FLOW_MAP_HPP_INCLUDED


#include "NonCopyable.hpp"

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <array>

#include "ShaderProgram.hpp"


/*! \class FlowMap
 * Class for handling, displaying and editing a 2D vector map.
 * The map is stored in a OpenGL texture.
 * The x values are stored in the red channel, the y values in the green one.
 * The stored vectors are in the [-1,1]x[-1,1] range, and of maximum length 1.
 * The map can be displayed in two ways : in colors (Map) or as a field of arrows.
 */
class FlowMap : public NonCopyable
{
public:
	/*! Constructor
	* If the parameters are invalid (size not matching, empty buffer...),
	* a default 32x32 buffer is used.
	*/
	FlowMap(glm::uvec2 const& bufferSize, std::vector<glm::vec2> const& flowBuffer);
	virtual ~FlowMap();

	inline glm::uvec2 const& getSize() { return _bufferSize; }

	/* Slow since it reads back the data from the GPU */
	void getData(glm::uvec2& bufferSize, std::vector<glm::vec2>& flowBuffer) const;
	
	/*! \param : pos normalized
	 * \param movement : normalized
	 * \param brushSize : normalized
	 * \param setNull : if true, the flow will locally be set to (0,0) */
	void changeLocally(glm::vec2 const& pos, glm::vec2 const& movement, glm::vec2 const& brushSize, bool setNull=false);

	/*! Linearily interpolates between the current flow and the initial flow.
	 * \param factor : will be clamped between 0 and 1 */
	void reset(float factor);

	/*! Displays the flow as a color map */
	void drawMap() const;

	/* Display the flow as a field of arrows */
	void drawArrows(glm::ivec2 const& screenSize) const;

	GLuint getTextureId() const { return currBufferId(); }

private:
	void switchBuffer();
	GLuint currBufferId() const;
	GLuint prevBufferId() const;

private:
	glm::uvec2 _bufferSize;
	const glm::uvec2 _nbArrows;

	GLuint _emptyVAO = 0u;
	GLuint _updateFBO = 0u;
	GLuint _initTexture = 0u;
	std::array<GLuint, 2> _flowTexture;
	unsigned int _currBufferNo; // in 0.._buffersId.size()-1

	GLuint _arrowsVAO = 0u;
	GLuint _arrowVBO = 0u;
	GLuint _arrowsPosVBO = 0u;

	mutable ShaderProgram _drawMapShader;
	mutable ShaderProgram _drawArrowsShader;
	mutable ShaderProgram _changeShader;
	mutable ShaderProgram _resetShader;
};

#endif // FLOW_MAP_HPP_INCLUDED