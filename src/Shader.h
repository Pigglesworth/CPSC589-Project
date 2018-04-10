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
    void useShader(T... uniforms)
    {
        glUseProgram(programID);
        setUniforms((uniforms)...);
    }

    void setUniform(const char* name, glm::mat4 val);
    void setUniform(const char* name, glm::vec3 val);
    void setUniform(const char* name, glm::vec4 val);
    void setUniform(const char* name, float val);

    template <class... T>
    void setUniforms(T... args)
    {
        setUniformsHelper((args)...);
    }

private:
    GLuint loadShader(const std::string& file, GLenum shaderType);

    void setUniformsHelper();

    template <class A, class B, class... T>
    void setUniformsHelper(A name, B val, T... rest)
    {
        setUniform((name),(val));
        setUniformsHelper((rest)...);
    }


    GLuint programID;
};