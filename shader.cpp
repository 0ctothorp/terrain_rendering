#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "shader.hpp"

Shader::Shader(const string &vertexShaderPath, const string &fragmentShaderPath) {
    string vertexShaderCode = LoadShaderFile(vertexShaderPath);
    const GLchar *vertexShaderCodeGLcharPtr = vertexShaderCode.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCodeGLcharPtr, nullptr);

    string fragmentShaderCode = LoadShaderFile(fragmentShaderPath);
    const GLchar *fragmentShaderCodeGLcharPtr = fragmentShaderCode.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCodeGLcharPtr, nullptr);

    CompileShader(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

string Shader::LoadShaderFile(string path) {
    string shaderCode;
    ifstream shaderFile;
    shaderFile.exceptions(ifstream::badbit | ifstream::failbit);
    try {
        shaderFile.open(path);
        stringstream shaderStream;
        // Read file's buffer contents into stream
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch(const ifstream::failure &e) {
        // @Refactor: e.what() gives poor error description; better w/ strerror(errno)
        cerr << "[EXCEPTION: Shader::LoadShaderFile] " << e.what() << '\n'
             << e.code() << '\n';
        exit(1);
    }
    return shaderCode;
}

void Shader::CompileShader(GLuint vertexShader, GLuint fragmentShader) {
    GLint result = GL_FALSE;
    int infoLogLength;

    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0 && !result) {
        vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(vertexShader, infoLogLength, nullptr, &vertexShaderErrorMessage[0]);
        cerr << "[VERTEX SHADER COMPILATION ERROR] " << &vertexShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    // @Refactor: Code repetition.
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0 && !result) {
        vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]);
        cerr << "[FRAGMENT SHADER COMPILATION ERROR] " << &fragmentShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    programId = glCreateProgram();
    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
    if(infoLogLength > 0 && !result) {
        vector<char> errorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programId, infoLogLength, nullptr, &errorMessage[0]);
        cout << "[SHADER LINKING ERROR] " << &errorMessage[0] << '\n';
        exit(1);
    }
}

GLuint Shader::GetProgramId() {
    return programId;
}