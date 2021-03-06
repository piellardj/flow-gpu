#ifndef IO_INCLUDED_HPP
#define IO_INCLUDED_HPP


#include <string>
#include <vector>

#include "glm.hpp"


namespace IO
{
	bool loadAlphaMask(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer);
	
	bool load32bitImage(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer);

	bool loadFlowMap(std::string const& filename, glm::uvec2& bufferSize, std::vector<glm::vec2>& buffer);
	bool saveFlowMap(std::string const& filename, glm::uvec2 const& bufferSize, std::vector<glm::vec2> const& buffer);

	bool loadFileToStr(std::string const& filename, std::string& fileStr);
}

#endif // IO_INCLUDED_HPP