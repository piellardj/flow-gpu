#include "GLHelper.hpp"


#include <string>
#include <iostream>
#include <array>
#include <utility>

#include "ShaderProgram.hpp"


void gl_CheckError(const char* file, unsigned int line, const char* expression)
{
    GLenum errorCode = glGetError();
    while (errorCode != GL_NO_ERROR) {
        std::string fileString = file;
        std::string error = "Unknown error";
        std::string description  = "No description";

        switch (errorCode) {
            case GL_INVALID_ENUM:
            {
                error = "GL_INVALID_ENUM";
                description = "An unacceptable value has been specified for an enumerated argument.";
                break;
            }

            case GL_INVALID_VALUE:
            {
                error = "GL_INVALID_VALUE";
                description = "A numeric argument is out of range.";
                break;
            }

            case GL_INVALID_OPERATION:
            {
                error = "GL_INVALID_OPERATION";
                description = "The specified operation is not allowed in the current state.";
                break;
            }

            case GL_STACK_OVERFLOW:
            {
                error = "GL_STACK_OVERFLOW";
                description = "This command would cause a stack overflow.";
                break;
            }

            case GL_STACK_UNDERFLOW:
            {
                error = "GL_STACK_UNDERFLOW";
                description = "This command would cause a stack underflow.";
                break;
            }

            case GL_OUT_OF_MEMORY:
            {
                error = "GL_OUT_OF_MEMORY";
                description = "There is not enough memory left to execute the command.";
                break;
            }

//            case GLEXT_GL_INVALID_FRAMEBUFFER_OPERATION:
//            {
//                error = "GL_INVALID_FRAMEBUFFER_OPERATION";
//                description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
//                break;
//            }
        }

        // Log the error
        std::cerr << "OpenGL error in "
                  << fileString.substr(fileString.find_last_of("\\/") + 1) << "(" << line << ")."
                  << "\nExpression:\n   " << expression
                  << "\nError description:\n   " << error << "\n   " << description << "\n"
                  << std::endl;

        errorCode = glGetError();
    }
}


static std::array<std::pair<unsigned int, GLenum>, 9> UNITS =
{ {
	{ 0, GL_TEXTURE0 },{ 1, GL_TEXTURE1 },{ 2, GL_TEXTURE2 },
	{ 3, GL_TEXTURE3 },{ 4, GL_TEXTURE4 },{ 5, GL_TEXTURE5 },
	{ 6, GL_TEXTURE6 },{ 7, GL_TEXTURE7 },{ 8, GL_TEXTURE8 }
} };

TextureBinder::TextureBinder():
	_currUnit(0u)
{
}

TextureBinder::~TextureBinder()
{
	if (_currUnit > 0u) {
		GLCHECK(glBindTexture(GL_TEXTURE_2D, 0u));
	}
}

bool TextureBinder::bindTexture(ShaderProgram& shader, std::string const& name, GLuint textureId)
{
	if (_currUnit >= TextureBinder::MAX_UNITS) {
		std::cerr << "Couldn't bind texture: only 9 units." << std::endl;
		return false;
	}

	GLuint texULoc = shader.getUniformLocation(name);
	if (texULoc == ShaderProgram::nullLocation) {
		std::cerr << "Couldn't find texture uniform '" << name << "'." << std::endl;
		return false;
	}

	GLCHECK(glUniform1i(texULoc, UNITS[_currUnit].first));
	GLCHECK(glActiveTexture(UNITS[_currUnit].second));
	GLCHECK(glBindTexture(GL_TEXTURE_2D, textureId));
	_currUnit++;
	return true;
}