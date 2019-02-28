#include "shader_program.h"

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath) {
    try {
        vertexCode_ = readFileContentsFromPath(vertexShaderPath);
        fragmentCode_ = readFileContentsFromPath(fragmentShaderPath);
    } catch (std::exception &e) {
        std::cerr << "Failed to read shader file contents.\n"<< e.what() << std::endl;
    }
}

void ShaderProgram::use() {
    glUseProgram(programId_);
}

int ShaderProgram::link() {
    int success;
    char infoLog[512];
    int vertexId, fragmentId;

    vertexId = compileShader(vertexCode_, GL_VERTEX_SHADER);
    fragmentId = compileShader(fragmentCode_, GL_FRAGMENT_SHADER);

    programId_ = glCreateProgram();
    glAttachShader(programId_, vertexId);
    glAttachShader(programId_, fragmentId);
    glLinkProgram(programId_);
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId_, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed.\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

    return programId_;
}

int ShaderProgram::compileShader(std::string shaderCode, GLenum shaderType) {
    int success;
    char infoLog[512];
    const char* str = shaderCode.c_str();
    int id = glCreateShader(shaderType);

    glShaderSource(id, 1, &str, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed.\n" << infoLog << std::endl;
    }

    return id;
}

std::string ShaderProgram::readFileContentsFromPath(const char* path) {
    std::ifstream file(path);
    file.exceptions(std::ios::badbit | std::ios::failbit);
    if(file.bad()) {
        throw std::ios::failure("Failed to open file at [" + std::string(path) + "]");
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        (std::istreambuf_iterator<char>()));

    std::cout << "INFO: Loaded shader from [" << path << "].\n" << content << std::endl << std::endl;
    file.close();

    return content;
}