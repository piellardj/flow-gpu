#ifndef PICKING_TEXTURE_HPP_INCLUDED
#define PICKING_TEXTURE_HPP_INCLUDED


#include "NonCopyable.hpp"
#include "Background.hpp"
#include "glm.hpp"
#include "ShaderProgram.hpp"

#include <memory>


class PickingTexture: public NonCopyable
{
public:
	static const uint8_t NO_LEVEL = 255u;

public:
	PickingTexture(glm::uvec2 const& resolution);
	virtual ~PickingTexture();

	uint8_t getLayer(glm::vec2 const& coords) const;

	void addBackground(Background& background, uint8_t layer);
	void lock();

private:
	void freeOpenGLResources();

private:
	const glm::uvec2 _bufferSize;
	std::vector<uint8_t> _layerBuffer;

	bool _locked;

	GLuint _emptyVAO = 0u;
	GLuint _FBO = 0u;
	GLuint _layerTexture = 0u;

	std::unique_ptr<ShaderProgram> _buildingShader;
};

#endif // PICKING_TEXTURE_HPP_INCLUDED