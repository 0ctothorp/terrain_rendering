#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "shader.hpp"
#include "glDebug.hpp"

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
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch(const ifstream::failure &e) {
        cerr << "[EXCEPTION: Shader::LoadShaderFile] " << e.what() << '\n' << e.code() << '\n';
        exit(1);
    }
    return shaderCode;
}

void Shader::CompileShader(GLuint vertexShader, GLuint fragmentShader) {
    GLint result = GL_FALSE;
    int infoLogLength;

    GL_CHECK(glCompileShader(vertexShader));
    GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result));
    GL_CHECK(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        GL_CHECK(glGetShaderInfoLog(vertexShader, infoLogLength, nullptr, &vertexShaderErrorMessage[0]));
        cerr << "[VERTEX SHADER COMPILATION ERROR] " << &vertexShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    GL_CHECK(glCompileShader(fragmentShader));
    GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result));
    GL_CHECK(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        GL_CHECK(glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]));
        cerr << "[FRAGMENT SHADER COMPILATION ERROR] " << &fragmentShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    programId = glCreateProgram();
    GL_CHECK(glAttachShader(programId, vertexShader));
    GL_CHECK(glAttachShader(programId, fragmentShader));
    
    GL_CHECK(glLinkProgram(programId));
    GL_CHECK(glGetProgramiv(programId, GL_LINK_STATUS, &result));
    GL_CHECK(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        vector<char> errorMessage(infoLogLength + 1);
        GL_CHECK(glGetProgramInfoLog(programId, infoLogLength, nullptr, &errorMessage[0]));
        cout << "[SHADER LINKING ERROR] " << &errorMessage[0] << '\n';
        exit(1);
    }
}

void Shader::Use() const {
    GL_CHECK(glUseProgram(programId));
}

GLuint Shader::GetUniformLocation(const string &uniform) const {
    return GL_CHECK(glGetUniformLocation(programId, uniform.c_str()));
}
