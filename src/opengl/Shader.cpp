
#include <iostream>
#include <fstream>
#include <sstream>
#include "Shader.h"
#include "glad/glad.h"

static unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error Shader Compilation Failed\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

Shader::Shader(const char* vertexCode, const char* fragmentCode)
{
    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    Id = glCreateProgram();
    glAttachShader(Id, vertex);
    glAttachShader(Id, fragment);
    glLinkProgram(Id);
}

void Shader::use() const
{
    glUseProgram(Id);
}

unsigned int Shader::getId() const
{
    return Id;
}

Shader::~Shader()
{
    glUseProgram(0);
    glDeleteProgram(Id);
}

void Shader::stop()
{
    glUseProgram(0);
}

Shader* Shader::load(const char* vertexPath, const char* fragmentPath)
{
    std::ostringstream vertexstream;
    std::ifstream vertexFile;
    vertexFile.open(vertexPath);
    vertexstream << vertexFile.rdbuf();
    std::ostringstream fragmentstream;
    std::ifstream fragmentFile;
    fragmentFile.open(fragmentPath);
    fragmentstream << fragmentFile.rdbuf();
    return new Shader(vertexstream.str().c_str(), fragmentstream.str().c_str());
}
