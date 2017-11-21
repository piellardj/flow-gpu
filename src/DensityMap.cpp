#include "DensityMap.hpp"


DensityMap::DensityMap(glm::uvec2 size, std::vector<uint8_t> const& density)
{
	const glm::uvec2 defaultDensitySize(1u, 1u);
	const std::vector<uint8_t> defaultDensity = { 255u };

	std::vector<uint8_t> const* actualDensity = &density;

	if (density.empty() || density.size() != size.x * size.y) {
		size = glm::uvec2(1u, 1u);
		actualDensity = &defaultDensity;
	}
	
	/* Automatic uint8_t to float cast*/
	std::vector<float> fDensity(actualDensity->begin(), actualDensity->end());
	_sampler.reset(new Sampler2D(size, fDensity));
}

std::vector<glm::vec2> DensityMap::sample(unsigned int nb)
{
	std::vector<glm::vec2> pos;

	if (_sampler) {
		for (unsigned int i = 0u; i < nb; ++i) {
			pos.emplace_back(_sampler->sample());
		}
	}

	return pos;
}