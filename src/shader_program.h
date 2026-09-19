#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

using namespace std;

#include <string>
#include <fstream>
#include <iostream>
#include <glad/glad.h>

class ShaderProgram {
public:
    ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, const string& defines = "");
    void link();
    void use();
    int getId();

private:
    int programId_;
    string vertexCode_;
    string fragmentCode_;

    string readFileContentsFromPath(const char* path);
    string insertAfterVersionDirective(const string& code, const string& defines);
    int compileShader(string shaderCode, GLenum shaderType);
};

#endif
