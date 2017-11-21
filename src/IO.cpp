#include "IO.hpp"


#include <iostream>
#include <iomanip>
#include <fstream>

#include <SFML/Graphics/Image.hpp>


namespace IO
{
	bool loadAlphaMask(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer)
	{
		sf::Image img;
		if (!img.loadFromFile(filename)) {
			std::cerr << "Failed: couldn't open image '" << filename << "'." << std::endl;
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
		return true;
	}

	bool load32bitImage(std::string const& filename, glm::uvec2& bufferSize, std::vector<uint8_t>& buffer)
	{
		sf::Image img;
		if (!img.loadFromFile(filename)) {
			std::cerr << "Failed: couldn't open image '" << filename << "'." << std::endl;
			bufferSize = glm::uvec2(0u);
			buffer.clear();
			return false;
		}
		img.flipVertically();

		bufferSize = glm::uvec2(img.getSize().x, img.getSize().y);
		buffer.resize(4 * bufferSize.x * bufferSize.y);

		unsigned int iT = 0u;
		for (unsigned int iY = 0u; iY < bufferSize.y; ++iY) {
			for (unsigned int iX = 0u; iX < bufferSize.x; ++iX) {
				buffer[4 * iT + 0] = img.getPixel(iX, iY).r;
				buffer[4 * iT + 1] = img.getPixel(iX, iY).g;
				buffer[4 * iT + 2] = img.getPixel(iX, iY).b;
				buffer[4 * iT + 3] = img.getPixel(iX, iY).a;
				++iT;
			}
		}
		return true;
	}

	bool loadFlowMap(std::string const& filename, glm::uvec2& bufferSize, std::vector<glm::vec2>& buffer)
	{
		std::ifstream file;
		file.open(filename);
		if (!file.is_open()) {
			std::cout << "Failed: couldn't open file '" << filename << "'";
			bufferSize = glm::uvec2(0u);
			buffer.clear();
			return false;
		}

		file >> bufferSize.x >> bufferSize.y;

		buffer.resize(bufferSize.x * bufferSize.y);
		for (glm::vec2& v : buffer) {
			if (file.eof()) {
				std::cout << "Failed: file '" << filename << "' is incomplete, aborting." << std::endl;
				bufferSize = glm::uvec2(0u);
				buffer.clear();
				file.close();
				return false;
			}

			file >> v.x >> v.y;
		}
		return true;
	}

	bool saveFlowMap(std::string const& filename, glm::uvec2 const& bufferSize, std::vector<glm::vec2> const& buffer)
	{
		std::cout << "Saving flowMap of size (" << bufferSize.x << "x" << bufferSize.y << ") to '" << filename << "'...\n";

		std::ofstream file;
		file.open(filename, std::ofstream::out | std::ofstream::trunc);
		if (!file.is_open()) {
			std::cout << "Failed: couldn't open file." << std::endl;
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