#ifndef SHADER_HPP_INCLUDED
#define SHADER_HPP_INCLUDED


#include <string>

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "NonCopyable.hpp"


/**@brief Basic class for vertex, geometry and fragment shaders.
 */
class Shader: public NonCopyable
{
    public:
        /**@brief Creates an empty shader. */
        Shader();
        virtual ~Shader();

        /**@brief Move constructor */
        Shader (Shader &&rvalue) noexcept;

        /**@brief Move assignment operator. */
        Shader& operator=(Shader &&rvalue) noexcept;

        /**@brief Attempts to (re)load shader from file.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader is not altered. */
        bool loadFromFile(std::string const& shaderFilename, GLuint shaderType);

        /**@brief Attempts to (re)load shader from memory.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader is not altered. */
        bool loadFromString(std::string const& shader, GLuint shaderType);

        bool isValid() const;

        /**@brief Returns OpenGL native handle. */
        GLuint Id();

        GLuint Type() const;
        std::string TypeStr() const;

        std::string const& Text() const;


    private:
        GLuint _shaderId;
        GLuint _shaderType;
        std::string _shaderText;
};

#endif // SHADER_HPP_INCLUDED
