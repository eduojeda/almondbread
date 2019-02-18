#include "shader_loader.h"

ShaderLoader::ShaderLoader(const char* vertexShaderPath, const char* fragmentShaderPath) {
    vertexCode = readFileContentsFromPath(vertexShaderPath);
    fragmentCode = readFileContentsFromPath(fragmentShaderPath);
}

int ShaderLoader::createProgram() {
    int success;
    char infoLog[512];
    int programId;
    int vertexId, fragmentId;

    vertexId = compileShader(vertexCode, GL_VERTEX_SHADER);
    fragmentId = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(programId, 512, NULL, infoLog);
        std::cout << "ERROR: Shader program linking failed.\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

    return programId;
}

int ShaderLoader::compileShader(std::string shaderCode, GLenum shaderType) {
    int success;
    char infoLog[512];
    const char *str = shaderCode.c_str();
    unsigned int id = glCreateShader(shaderType);

    glShaderSource(id, 1, &str, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR: Shader compilation failed.\n" << infoLog << std::endl;
    }

    return id;
}

std::string ShaderLoader::readFileContentsFromPath(const char* path) {
    try {
        std::ifstream file(path);
        file.exceptions(std::ios::badbit | std::ios::failbit);
        if(file.bad()) {
            std::cout << "ERROR: Failed to open file at [" << path << "].\n" << std::endl;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));

        std::cout << "INFO: Loaded shader from [" << path << "].\n" << content << std::endl << std::endl;
        file.close();

        return content;
    } catch (std::ifstream::failure e) {
        std::cout << "ERROR: Failed to read shader file contents from [" << path << "].\n"<< e.what() << std::endl;
        return nullptr;
    }
}