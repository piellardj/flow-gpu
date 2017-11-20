#ifndef FLOW_MAP_HPP_INCLUDED
#define FLOW_MAP_HPP_INCLUDED


#include "NonCopyable.hpp"

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include <array>

#include "ShaderProgram.hpp"


class FlowMap : public NonCopyable
{
public:
	FlowMap(glm::uvec2 const& bufferSize, std::vector<glm::vec2> flowBuffer);
	virtual ~FlowMap();

	void getData(glm::uvec2& bufferSize, std::vector<glm::vec2>& flowBuffer) const;
	
	void addFlow(glm::vec2 const& pos, glm::vec2 const& flow, float brushSize=0.1f);

	void drawMap() const;
	void drawArrows() const;

	GLuint getTextureId() const { return currBufferId(); }

private:
	void switchBuffer();
	GLuint currBufferId() const;
	GLuint prevBufferId() const;

private:
	const glm::uvec2 _bufferSize;
	const glm::uvec2 _nbArrows;

	GLuint _emptyVAO = 0u;
	GLuint _updateFBO = 0u;
	std::array<GLuint, 2> _flowTexture;
	unsigned int _currBufferNo; // in 0.._buffersId.size()-1

	GLuint _arrowsVAO = 0u;
	GLuint _arrowVBO = 0u;
	GLuint _arrowsPosVBO = 0u;

	mutable ShaderProgram _drawMapShader;
	mutable ShaderProgram _drawArrowsShader;
	mutable ShaderProgram _addShader;
};


#endif // FLOW_MAP_HPP_INCLUDED