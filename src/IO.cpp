#include "IO.hpp"


#include <iostream>
#include <iomanip>
#include <fstream>

#include <SFML/Graphics/Image.hpp>


namespace IO
{
	bool loadAlphaMask(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer)
	{
		std::cout << "Loading alpha mask from '" << filename << "'...\n";
		sf::Image img;
		if (!img.loadFromFile(filename)) {
			std::cerr << "failed: couldn't open image '" << filename << "'." << std::endl;
			bufferSize = glm::uvec2(0u);
			buffer.clear();
			return false;
		}
		img.flipVertically();

		bufferSize = glm::uvec2(img.getSize().x, img.getSize().y);
		buffer.resize(bufferSize.x * bufferSize.y);

		unsigned int iT = 0u;
		for (unsigned int iY = 0u; iY < bufferSize.y; ++iY) {
			for (unsigned int iX = 0u; iX < bufferSize.x; ++iX) {
				buffer[iT] = img.getPixel(iX, iY).r;
				++iT;
			}
		}

		std::cout << "done!" << std::endl;
		return true;
	}

	bool load24bitImage(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer)
	{
		std::cout << "Loading 24bit image from from '" << filename << "'...\n";

		sf::Image img;
		if (!img.loadFromFile(filename)) {
			std::cerr << "failed: couldn't open image '" << filename << "'." << std::endl;
			bufferSize = glm::uvec2(0u);
			buffer.clear();
			return false;
		}
		img.flipVertically();

		bufferSize = glm::uvec2(img.getSize().x, img.getSize().y);
		buffer.resize(3 * bufferSize.x * bufferSize.y);

		unsigned int iT = 0u;
		for (unsigned int iY = 0u; iY < bufferSize.y; ++iY) {
			for (unsigned int iX = 0u; iX < bufferSize.x; ++iX) {
				buffer[3 * iT + 0] = img.getPixel(iX, iY).r;
				buffer[3 * iT + 1] = img.getPixel(iX, iY).g;
				buffer[3 * iT + 2] = img.getPixel(iX, iY).b;
				++iT;
			}
		}

		std::cout << "done!" << std::endl;
		return true;
	}

	bool loadFlowMap(std::string const& filename, glm::uvec2& bufferSize, std::vector<glm::vec2>& buffer)
	{
		std::cout << "Loading flowMap from '" << filename << "'...\n";
		std::ifstream file;
		file.open(filename);
		if (!file.is_open()) {
			std::cout << "failed: couldn't open file '" << filename << "'";
			bufferSize = glm::uvec2(0u);
			buffer.clear();
			return false;
		}

		file >> bufferSize.x >> bufferSize.y;
		std::cout << "flowMap: loading buffer of size " << bufferSize.x << "x" << bufferSize.y << "\n";

		buffer.resize(bufferSize.x * bufferSize.y);
		for (glm::vec2& v : buffer) {
			if (file.eof()) {
				std::cout << "failed: file '" << filename << "' is incomplete, aborting." << std::endl;
				bufferSize = glm::uvec2(0u);
				buffer.clear();
				file.close();
				return false;
			}

			file >> v.x >> v.y;
		}

		std::cout << " done!" << std::endl;
		return true;
	}

	bool saveFlowMap(std::string const& filename, glm::uvec2 const& bufferSize, std::vector<glm::vec2> const& buffer)
	{
		std::cout << "Saving flowMap of size (" << bufferSize.x << "x" << bufferSize.y << ") to '" << filename << "'...\n";

		std::ofstream file;
		file.open(filename, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open()) {
			std::cout << "failed: couldn't open file." << std::endl;
			return false;
		}

		file << bufferSize.x << " " << bufferSize.y << std::endl;
		file << std::setprecision(std::numeric_limits<float>::digits10) << std::fixed;
		for (glm::vec2 const& v : buffer) {
			file << v.x << " " << v.y << "\n";
		}
		file.close();

		std::cout << "done!" << std::endl;

		return true;
	}
}