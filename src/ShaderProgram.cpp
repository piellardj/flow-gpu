#include "ShaderProgram.hpp"


#include <iostream>

#include "GLHelper.hpp"


const GLuint ShaderProgram::nullLocation = (GLuint)(-1);


ShaderProgram::ShaderProgram():
            _programId(0),
            _valid(false)
{
}

ShaderProgram::~ShaderProgram()
{
    if (glIsProgram(_programId) == GL_TRUE) {
        GLCHECK(glDeleteProgram(_programId));
    }
}

bool ShaderProgram::loadFromFile(std::string const& vertexFilename,
                                 std::string const& fragmentFilename)
{
    Shader vertexShader, fragmentShader;
    if (!vertexShader.loadFromFile(vertexFilename, GL_VERTEX_SHADER)
        || !fragmentShader.loadFromFile(fragmentFilename, GL_FRAGMENT_SHADER)){
        std::cerr << "Couldn't load new shader program. Shader program unchanged." << std::endl;
        return false;
    }

    std::vector<Shader> shaders;
    shaders.push_back(std::move(vertexShader));
    shaders.push_back(std::move(fragmentShader));
    return buildProgram(shaders);
}

bool ShaderProgram::loadFromFile(std::string const& vertexFilename,
                                 std::string const& fragmentFilename,
                                 std::string const& geometryFilename)
{
    Shader vertexShader, fragmentShader, geometryShader;
    if (!vertexShader.loadFromFile(vertexFilename, GL_VERTEX_SHADER)
        || !fragmentShader.loadFromFile(fragmentFilename, GL_FRAGMENT_SHADER)
        || !geometryShader.loadFromFile(geometryFilename, GL_GEOMETRY_SHADER)) {
        std::cerr << "Couldn't load new shader program. Shader program unchanged." << std::endl;
        return false;
    }

    std::vector<Shader> shaders;
    shaders.push_back(std::move(vertexShader));
    shaders.push_back(std::move(fragmentShader));
    shaders.push_back(std::move(geometryShader));
    return buildProgram(shaders);
}

bool ShaderProgram::loadFromString(std::string const& vertexShaderText,
                                   std::string const& fragmentShaderText)
{
    Shader vertexShader, fragmentShader;
    if (!vertexShader.loadFromString(vertexShaderText, GL_VERTEX_SHADER)
        || !fragmentShader.loadFromString(fragmentShaderText, GL_FRAGMENT_SHADER)){
        std::cerr << "Couldn't load new shader program. Shader program unchanged." << std::endl;
        return false;
    }

    std::vector<Shader> shaders;
    shaders.push_back(std::move(vertexShader));
    shaders.push_back(std::move(fragmentShader));
    return buildProgram(shaders);
}

bool ShaderProgram::loadFromString(std::string const& vertexShaderText,
                                   std::string const& fragmentShaderText,
                                   std::string const& geometryShaderText)
{
    Shader vertexShader, fragmentShader, geometryShader;
    if (!vertexShader.loadFromString(vertexShaderText, GL_VERTEX_SHADER)
        || !fragmentShader.loadFromString(fragmentShaderText, GL_FRAGMENT_SHADER)
        || !geometryShader.loadFromString(geometryShaderText, GL_GEOMETRY_SHADER)){
        std::cerr << "Couldn't load new shader program. Shader program unchanged." << std::endl;
        return false;
    }

    std::vector<Shader> shaders;
    shaders.push_back(std::move(vertexShader));
    shaders.push_back(std::move(fragmentShader));
    shaders.push_back(std::move(geometryShader));
    return buildProgram(shaders);
}

void ShaderProgram::bind(ShaderProgram& program)
{
    GLCHECK(glUseProgram(program._programId));
}

void ShaderProgram::unbind()
{
    GLCHECK(glUseProgram(0));
}

bool ShaderProgram::isValid() const
{
    return _valid;
}

GLuint ShaderProgram::getUniformLocation (std::string const& name) const
{
   std::unordered_map<std::string, GLint>::const_iterator result = _uniforms.find(name);
    if (result != _uniforms.end())
        return result->second;
    return ShaderProgram::nullLocation;
}

GLuint ShaderProgram::getAttribLocation (std::string const& name) const
{
    std::unordered_map<std::string, GLint>::const_iterator result = _attributes.find(name);
    if (result != _attributes.end())
        return result->second;
    return ShaderProgram::nullLocation;
}

GLuint ShaderProgram::programId()
{
    return _programId;
}

bool ShaderProgram::buildProgram (std::vector<Shader>& shaders)
{
    for (Shader const& shader : shaders) {
        if (!shader.isValid()) {
            std::cerr << "Couldn't load new shader program. Shader program unchanged." << std::endl;
            return false;
        }
    }

    /* We save the current shader's ID for easy restoration if building failure */
    const GLuint previousProgramId = _programId;

    GLCHECK(_programId = glCreateProgram());
    if (_programId == 0) {
        std::cerr << "Couldn't create new shader program. Shader program unchanged." << std::endl;
        _programId = previousProgramId;
        return false;
    }

    for (Shader& shader : shaders) {
        GLCHECK(glAttachShader(_programId, shader.Id()));
    }
    GLCHECK(glLinkProgram(_programId));

     GLint linkStatus;
     GLCHECK(glGetProgramiv(_programId, GL_LINK_STATUS, &linkStatus));
     if (linkStatus == GL_FALSE) {
        std::cerr << "Couldn't link shader program made of ";
        for (Shader const& shader : shaders)
                std::cerr << shader.TypeStr() << " shader [" << shader.Text() << "],\n";

        std::cerr << " Shader program unchanged." << std::endl;
        _programId = previousProgramId;
        return false;
     }

     /* If the shader was already a shader program, free previous resources. */
    if (glIsProgram(previousProgramId)) {
        GLCHECK(glDeleteProgram(previousProgramId));
    }

    queryActiveAttributesLocation();
    queryActiveUniformsLocation();

    _valid = true;
    return true;
}

void ShaderProgram::queryActiveUniformsLocation()
{
    _uniforms.clear();
    _attributes.clear();

    /* Reading uniforms */
    GLint nbUniforms = 0, maxUniformNameLength = 0;
    GLCHECK(glGetProgramiv(_programId, GL_ACTIVE_UNIFORMS, &nbUniforms));
    GLCHECK(glGetProgramiv(_programId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength));
    std::vector<char> name(512);//maxUniformNameLength);

    for (GLint iU = 0 ; iU < nbUniforms ; ++iU) {
        GLsizei nameLength;
        GLenum uniformType;
        GLint uniformSize;
        GLuint uniformLocation;

        GLCHECK(glGetActiveUniform(_programId, iU, name.size(), &nameLength, &uniformSize, &uniformType, name.data()));
        GLCHECK(uniformLocation = glGetUniformLocation(_programId, name.data()));

        _uniforms.emplace(std::string(name.data()), uniformLocation);
    }
}

void ShaderProgram::queryActiveAttributesLocation()
{
    /* Reading attributes */
    GLint nbAttributes = 0, maxAttributeNameLength = 0;
    GLCHECK(glGetProgramiv(_programId, GL_ACTIVE_ATTRIBUTES, &nbAttributes));
    GLCHECK(glGetProgramiv(_programId, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeNameLength));
    std::vector<char> name(512);//maxAttributeNameLength);

    for (GLint iA = 0 ; iA < nbAttributes ; ++iA) {
        GLsizei nameLength;
        GLenum attribType;
        GLint attribSize;
        GLint attribLocation;

        GLCHECK(glGetActiveAttrib(_programId, iA, name.size(), &nameLength, &attribSize, &attribType, name.data()));
        GLCHECK(attribLocation = glGetAttribLocation(_programId, name.data()));

        _attributes.emplace(std::string(name.data()), attribLocation);
    }
}
