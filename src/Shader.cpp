#include "Shader.hpp"


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "GLHelper.hpp"


static std::string loadFile(std::string const& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: couldn't open file \"" << filename << "\"." << std::endl;
        return std::string();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    file.close();

    return ss.str();
}

static std::string TypeToStr(GLuint shaderType)
{
    if (shaderType == GL_VERTEX_SHADER)
        return "vertex";
    else if (shaderType == GL_FRAGMENT_SHADER)
        return "fragment";
    else if (shaderType == GL_GEOMETRY_SHADER)
        return "geometry";

    return "UNKNOWN";
}


Shader::Shader():
            _shaderId(0)
{
}

Shader::~Shader()
{
    GLCHECK(glDeleteShader(_shaderId));
}

Shader::Shader (Shader &&rvalue) noexcept
{
    _shaderId = rvalue._shaderId;
    _shaderType = rvalue._shaderType;
    _shaderText = rvalue._shaderText;

    rvalue._shaderId = 0;
}

Shader& Shader::operator=(Shader &&rvalue) noexcept
{
    std::swap(_shaderId, rvalue._shaderId);
    std::swap(_shaderType, rvalue._shaderType);
    std::swap(_shaderText, rvalue._shaderText);

    return *this;
}

bool Shader::loadFromFile(std::string const& shaderFilename, GLuint shaderType)
{
    std::string shader = loadFile(shaderFilename);
    return loadFromString(shader, shaderType);
}

bool Shader::loadFromString(std::string const& shader, GLuint shaderType)
{
    /* Check shader type */
    if (shaderType != GL_VERTEX_SHADER
        && shaderType != GL_FRAGMENT_SHADER
        && shaderType != GL_GEOMETRY_SHADER) {
        std::cerr << "Error: only vertex, geometry and fragment shader are supported." << std::endl;
        return false;
    }

    const GLuint previousShaderId = _shaderId;
    std::string shaderTypeStr = TypeToStr(shaderType);

    /* Shader creation */
    GLCHECK(_shaderId = glCreateShader(shaderType));
    if (_shaderId == 0) {
        std::cerr << "Error: couldn't create " << shaderTypeStr << " shader." << std::endl;
        _shaderId = previousShaderId;
        return false;
    }

    /* Sending code */
    GLchar const* shaderString[1] = {shader.c_str()};
    GLint const shaderLength[1] = {(GLint)shader.size()};
    GLCHECK(glShaderSource(_shaderId, 1,  shaderString, shaderLength));

    /* Compilation */
    GLCHECK(glCompileShader(_shaderId));
    GLint compileStatus;
    GLCHECK(glGetShaderiv(_shaderId, GL_COMPILE_STATUS, &compileStatus));
    if (compileStatus == GL_FALSE) {
        std::cerr << "Error: couldn't compile " << shaderTypeStr << " shader." << std::endl;

        GLint logLen;
        GLCHECK(glGetShaderiv(_shaderId, GL_INFO_LOG_LENGTH, &logLen));
        if(logLen > 0) {
            std::vector<char> log(logLen);
            GLsizei written;
            GLCHECK(glGetShaderInfoLog(_shaderId, logLen, &written, log.data()));
            std::cerr << "== Shader log: ==\n" << log.data() << std::endl;
        }
        std::cerr << shader << std::endl;
        GLCHECK(glDeleteShader(_shaderId));
        return false;
    }

    _shaderType = shaderType;
    _shaderText = shader;
    GLCHECK(glDeleteShader(previousShaderId));
    return true;
}

bool Shader::isValid() const
{
    return _shaderId != 0;
}

GLuint Shader::Id()
{
    return _shaderId;
}

GLuint Shader::Type() const
{
    return _shaderType;
}

std::string Shader::TypeStr() const
{
    return TypeToStr(_shaderType);
}

std::string const& Shader::Text() const
{
    return _shaderText;
}
