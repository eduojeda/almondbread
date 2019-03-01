#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <fstream>
#include <iostream>

#include <glad/glad.h>

class ShaderProgram {
public:
    ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
    void link();
    void use();
    int programId_; // TODO make private and add uniform helpers!

private:
    std::string vertexCode_;
    std::string fragmentCode_;

    std::string readFileContentsFromPath(const char* path);
    int compileShader(std::string shaderCode, GLenum shaderType);
};

#endif