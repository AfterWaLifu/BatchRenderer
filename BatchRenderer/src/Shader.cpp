#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

Shader::Shader(const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename)
    :m_RendererID(0)
{
    m_RendererID = CreateShader(vertexShaderFilename, fragmentShaderFilename);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

std::string Shader::ReadFile(const std::string& filename)
{
    std::ifstream f(filename);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& code)
{
    unsigned int id = glCreateShader(type);
    const char* src = code.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        char* message = (char*)_malloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile shader:\n" <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << " shader\n" << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& m_VertShaderFilename, const std::string& m_FragShaderFilename)
{
    std::string vertCode = ReadFile(m_VertShaderFilename);
    std::string fragCode = ReadFile(m_FragShaderFilename);

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertCode);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragCode);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
