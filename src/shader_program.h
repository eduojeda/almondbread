#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include <glad/glad.h>

class ShaderProgram {
public:
    ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);
    void link();
    void use();
    int getId();

private:
    int programId_;
    string vertexCode_;
    string fragmentCode_;

    string readFileContentsFromPath(const char* path);
    int compileShader(string shaderCode, GLenum shaderType);
};

#endif