#ifndef GLHELPER_HPP_INCLUDED
#define GLHELPER_HPP_INCLUDED

#include <string>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

void gl_CheckError(const char* file, unsigned int line, const char* expression);

#define DEBUG
#ifdef DEBUG
    #define GLCHECK(expr) do { expr; gl_CheckError(__FILE__, __LINE__, #expr); } while (false)
#else
    #define GLCHECK(expr) (expr)
#endif // DEBUG


class ShaderProgram;
class TextureBinder
{
public:
	TextureBinder();
	~TextureBinder();
	bool bindTexture(ShaderProgram& shader, std::string const& name, GLuint textureId);

private:
	unsigned int _currUnit = 0u;
	static const unsigned int MAX_UNITS = 9u;
};

#endif // GLHELPER_HPP_INCLUDED
