#ifndef SHADER_LOADER_H
#define SHADER_LOADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <iostream>

class ShaderLoader {
private:
    std::string vertexCode;
    std::string fragmentCode;

    std::string readFileContentsFromPath(const char* path);
    int compileShader(std::string shaderCode, GLenum shaderType);

public:
    ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath);
    int createProgram();
};

#endif