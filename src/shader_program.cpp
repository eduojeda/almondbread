#include "shader_program.h"

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath, const string& defines) {
    try {
        vertexCode_ = insertAfterVersionDirective(readFileContentsFromPath(vertexShaderPath), defines);
        fragmentCode_ = insertAfterVersionDirective(readFileContentsFromPath(fragmentShaderPath), defines);
    } catch (exception &e) {
        cerr << "Failed to read shader file contents.\n"<< e.what() << endl;
    }
}

int ShaderProgram::getId() {
    return programId_;
}

void ShaderProgram::use() {
    glUseProgram(programId_);
}

void ShaderProgram::link() {
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
        cerr << "Shader program linking failed.\n" << infoLog << endl;
    }

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
}

int ShaderProgram::compileShader(string shaderCode, GLenum shaderType) {
    int success;
    char infoLog[512];
    const char* str = shaderCode.c_str();
    int id = glCreateShader(shaderType);

    glShaderSource(id, 1, &str, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        cerr << "Shader compilation failed.\n" << infoLog << endl;
    }

    return id;
}

string ShaderProgram::readFileContentsFromPath(const char* path) {
    ifstream file(path);
    file.exceptions(ios::badbit | ios::failbit);
    if(file.bad()) {
        throw ios::failure("Failed to open file at [" + string(path) + "]");
    }

    string content((istreambuf_iterator<char>(file)),
                        (istreambuf_iterator<char>()));

    cout << "INFO: Loaded shader code from [" << path << "].\n" << endl;
    file.close();

    return content;
}

// GLSL requires #version to be the first line, so extra definitions go right after it.
string ShaderProgram::insertAfterVersionDirective(const string& code, const string& defines) {
    if (defines.empty()) {
        return code;
    }

    string block = defines;
    if (block.back() != '\n') {
        block += '\n';
    }

    size_t firstLineEnd = code.find('\n');
    if (firstLineEnd == string::npos) {
        return code + "\n" + block;
    }

    return code.substr(0, firstLineEnd + 1) + block + code.substr(firstLineEnd + 1);
}
