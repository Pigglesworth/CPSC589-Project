#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const std::string& v, const std::string& f)
{
    GLuint vShader = loadShader(v, GL_VERTEX_SHADER);
    GLuint fShader = loadShader(f, GL_FRAGMENT_SHADER);

    programID = glCreateProgram();

    glAttachShader(programID, vShader);
    glAttachShader(programID, fShader);
    glLinkProgram(programID);
}




void Shader::setUniform(const char* name, glm::mat4 val)
{
    GLint loc = glGetUniformLocation(programID, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setUniform(const char* name, glm::vec4 val)
{
    GLint loc = glGetUniformLocation(programID, name);
    glUniform4fv(loc, 1, glm::value_ptr(val));
}

void Shader::setUniform(const char* name, glm::vec3 val)
{
    GLint loc = glGetUniformLocation(programID, name);
    glUniform3fv(loc, 1, glm::value_ptr(val));
}

void Shader::setUniform(const char* name, float val)
{
    GLint loc = glGetUniformLocation(programID, name);
    glUniform1f(loc,val);
}



void Shader::setUniformsHelper(){}

GLuint Shader::loadShader(const std::string& filepath, GLenum shaderType)
{
    std::ifstream file;
    file.open(filepath);
    if(!file)
        return -1;

    GLuint shader = glCreateShader(shaderType);

    std::stringstream shaderSource;
    shaderSource << file.rdbuf();

    std::string shaderString = shaderSource.str();
    GLint length = shaderString.size();
    const char* shaderStringPart2 = shaderString.c_str();

    glShaderSource(shader, 1, &shaderStringPart2, &length);
    glCompileShader(shader);

    GLint success;

    glGetObjectParameterivARB(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char buffer[256];
        glGetShaderInfoLog(shader,256,NULL,buffer);

        std::cout << buffer << std::endl;
    }        


    return shader;
}