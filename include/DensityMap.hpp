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

	inline float surface() const { return _surface; }

	std::vector<glm::vec2> computeInitPos(unsigned int nb);

private:
	std::unique_ptr<Sampler2D> _sampler;

	float _surface; // in [0,1], with 0: no surface, 1: full opacity square
};

#endif // DENSITY_MAP_HPP_INCLUDED