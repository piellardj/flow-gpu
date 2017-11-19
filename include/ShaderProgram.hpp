#ifndef SHADER_PROGRAM_HPP_INCLUDED
#define SHADER_PROGRAM_HPP_INCLUDED


#include <string>
#include <unordered_map>
#include <vector>

#include "glm.hpp"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>

#include "NonCopyable.hpp"
#include "Shader.hpp"


/**@brief Basic class for shader programs including vertex, geometry and fragment shaders.
 *
 * After the compilation, the class performs a resource introspection pass.
 * This is done once to avoid asking the GPU several times later.
 */
class ShaderProgram: public NonCopyable
{
    public:
        /**@brief Creates an invalid shader. */
        ShaderProgram();
        ~ShaderProgram();

        /**@brief Attempts to (re)load shader program from files.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader program is not altered. */
        bool loadFromFile(std::string const& vertexFilename,
                          std::string const& fragmentFilename);

        /**@brief Attempts to (re)load shader program from files.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader program is not altered. */
        bool loadFromFile(std::string const& vertexFilename,
                          std::string const& fragmentFilename,
                          std::string const& geometryFilename);

        /**@brief Attempts to (re)load shader program from memory.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader program is not altered. */
        bool loadFromString(std::string const& vertexShader,
                            std::string const& fragmentShader);

        /**@brief Attempts to (re)load shader program from memory.
         * If for some reason the compilation fails, a message is send to std::cerr and
         * the current shader program is not altered. */
        bool loadFromString(std::string const& vertexShader,
                            std::string const& fragmentShader,
                            std::string const& geometryShader);

        bool isValid() const;

        /**@brief Binds a valid shader so it can be used for drawing */
        static void bind(ShaderProgram& program);

        /**@brief Unbinds all shaders. */
        static void unbind();

        /**@brief Returns the uniform location.
         * If the uniform doesn't exist, ShaderProgram::nullLocation is returned. */
        GLuint getUniformLocation (std::string const& name) const;
        /**@brief Returns the attribute location.
         * If the attribute doesn't exist, ShaderProgram::nullLocation is returned. */
        GLuint getAttribLocation (std::string const& name) const;

        /**@brief Returns OpenGL native handle. */
        GLuint programId();

        static const GLuint nullLocation;

    private:
        bool buildProgram (std::vector<Shader>& shaders);

        void queryActiveUniformsLocation();
        void queryActiveAttributesLocation();

    private:
      GLuint _programId;
      bool _valid;

      std::unordered_map<std::string, GLint> _uniforms;
      std::unordered_map<std::string, GLint> _attributes;
};

#endif // SHADERPROGRAM_HPP_INCLUDED
