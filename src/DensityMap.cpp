#include "DensityMap.hpp"


DensityMap::DensityMap(glm::uvec2 size, std::vector<uint8_t> density)
{
	if (density.size() != size.x * size.y || density.empty()) {
		size = glm::uvec2(1u, 1u);
		density.clear();
		density.emplace_back(255u);
	}

	const float maxSurface = 255.f * static_cast<float>(size.x * size.y);
	float surface = 0.f;
	std::vector<float> fDensity(density.size());
	for (unsigned int i = 0u; i < density.size(); ++i) {
		fDensity[i] = static_cast<float>(density[i]);
		surface += fDensity[i];
	}
	_surface = surface / maxSurface;

	_sampler.reset(new Sampler2D(size, fDensity));
}

std::vector<glm::vec2> DensityMap::computeInitPos(unsigned int nb)
{
	std::vector<glm::vec2> pos;

	if (_sampler) {
		for (unsigned int i = 0u; i < nb; ++i) {
			pos.emplace_back(_sampler->sample());
		}
	}

	return pos;
}