#include "Sampler2D.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <algorithm>


static float random(float from = 0.f, float to = 1.f)
{
	static bool firstTime = true;
	if (firstTime) {
		firstTime = false;
		std::srand(static_cast<unsigned int>(std::time(NULL)));
	}

	float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	return from + r * (to - from);
}

/* Normalizes the line and returns the previous cumulated sum.
 * The line is assumed to be positive. */
static float normalizeLine(std::vector<float>& line)
{
	float total = 0.f;
	for (float f : line) {
		total += f;
	}
	if (total > 0.f) {
		for (float& f : line) {
			f /= total;
		}
	}
	return total;
}

/*! \return a if x=0, b if x=1, and linearily interpolates for other x values */
static float mix(float a, float b, float x)
{
	return x * b + (1.f - x) * a;
}

Sampler2D::Sampler2D(glm::uvec2 const& size, std::vector<float> density) :
	_size(size)
{
	bool invalid = false;
	if (density.size() != size.x * size.y) {
		std::cerr << "Error: the provided buffer size (" << density.size() << ") doesn't match the provided size (" << size.x << "x" << size.y << ")." << std::endl;
		invalid = true;
	}
	else if (density.empty()) {
		std::cerr << "Error: the provided buffer was empty." << std::endl;
		invalid = true;
	}

	if (invalid) {
		_size = glm::uvec2(1u, 1u);
		density.clear();
		density.emplace_back(1.f);
	}

	/* Density map is positive */
	for (float& f : density) {
		f = std::max(0.f, f);
	}

	/* Split the grid into normalized lines */
	for (unsigned int iLine = 0u; iLine < _size.y; ++iLine) {
		auto lineStart = density.cbegin() + iLine * _size.x;
		_normalizedLines.emplace_back(lineStart, lineStart + _size.x);
		_cumulatedLines.emplace_back(normalizeLine(_normalizedLines.back()));
	}
	normalizeLine(_cumulatedLines);
}

/*!
 * \brief Uses inverse method to generate a random sample from a 1D density map.
 * \param row : expected to be of normalized sum.
 * \return value in [0, 1] */
static float sample1D(std::vector<float> const& row)
{
	const float r = random(0.f, 1.f);

	unsigned int current = 0u;
	float total = row.front();
	while (r >= total && current < row.size()-1u) {
		current++;
		total += row[current];
	}

	return static_cast<float>(current) + (total - r) / row[current];
}

glm::vec2 Sampler2D::sample() const
{
	const float y = sample1D(_cumulatedLines);

	unsigned int iLine = std::min(static_cast<unsigned int>(y), _size.y-1u);
	const float x = sample1D(_normalizedLines[iLine]);

	return glm::vec2(x / static_cast<float>(_size.x), y / static_cast<float>(_size.y));
}