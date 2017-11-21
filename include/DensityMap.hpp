#ifndef DENSITY_MAP_HPP_INCLUDED
#define DENSITY_MAP_HPP_INCLUDED


#include <vector>
#include <cstdint>
#include <memory>

#include "glm.hpp"
#include "Sampler2D.hpp"


/*! \class DensityMap
 * Class for handling a discrete 2D probability (density) map and sampling it.
 */
class DensityMap
{
public:
	/*! Constructor
	 * If the parameters are invalid (size not matching, empty buffer...),
	 * a default uniform density is used.
	 */
	DensityMap(glm::uvec2 size, std::vector<uint8_t> const& density);

	/*! \return a random sample in [0,1]x[0,1] */
	std::vector<glm::vec2> sample(unsigned int nb);

private:
	std::unique_ptr<Sampler2D> _sampler;
};

#endif // DENSITY_MAP_HPP_INCLUDED