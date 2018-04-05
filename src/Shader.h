#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string& v, const std::string& f);
    
    template <class... T>
    void useShader(T&&... uniforms)
    {
        glUseProgram(programID);
        setUniforms(std::forward<T>(uniforms)...);
    }

    void setUniform(const char*&& name, glm::mat4&& val);
    void setUniform(const char*&& name, glm::vec3&& val);
    void setUniform(const char*&& name, glm::vec4&& val);
    void setUniform(const char*&& name, float val);

    template <class... T>
    void setUniforms(T&&... args)
    {
        setUniformsHelper(std::forward<T>(args)...);
    }

private:
    GLuint loadShader(const std::string& file, GLenum shaderType);

    void setUniformsHelper();

    template <class A, class B, class... T>
    void setUniformsHelper(A&& name, B&& val, T... rest)
    {
        setUniform(std::forward<A>(name), std::forward<B>(val));
        setUniformsHelper(std::forward<T>(rest)...);
    }


    GLuint programID;
};