#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <fstream>
#include <iostream>

#include <glad/glad.h>

class ShaderProgram {
public:
    ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
    int link();
    void use();

private:
    int programId_;
    std::string vertexCode_;
    std::string fragmentCode_;

    std::string readFileContentsFromPath(const char* path);
    int compileShader(std::string shaderCode, GLenum shaderType);
};

#endif