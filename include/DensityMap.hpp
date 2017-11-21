#ifndef DENSITY_MAP_HPP_INCLUDED
#define DENSITY_MAP_HPP_INCLUDED


#include <vector>
#include <cstdint>
#include <memory>

#include "glm.hpp"
#include "Sampler2D.hpp"


class DensityMap
{
public:
	DensityMap(glm::uvec2 size, std::vector<uint8_t> density);

	std::vector<glm::vec2> computeInitPos(unsigned int nb);

private:
	std::unique_ptr<Sampler2D> _sampler;
};

#endif // DENSITY_MAP_HPP_INCLUDED