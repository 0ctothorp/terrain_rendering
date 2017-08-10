#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "glDebug.hpp"


Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath, 
               const std::string& geomShaderPath) {
    GLuint vertexShader = GetShaderProgram(ShaderType_Vertex, vertexShaderPath);
    GLuint fragmentShader = GetShaderProgram(ShaderType_Fragment, fragmentShaderPath);
    GLuint geomShader = GetShaderProgram(ShaderType_Geometry, geomShaderPath);
    CompileShader(vertexShader, fragmentShader, geomShader);
    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));
    if(geomShader != 0) 
        GL_CHECK(glDeleteShader(geomShader));
}

GLuint Shader::GetShaderProgram(ShaderType_ shaderType, const std::string& shaderPath) {
    if(shaderPath == "") return 0;
    std::string shaderCodeStr = LoadShaderFile(shaderPath);
    const GLchar *shaderCodeGLcharPtr = shaderCodeStr.c_str();
    GLuint shaderProgram = CreateShaderProgramOfType(shaderType);
    glShaderSource(shaderProgram, 1, &shaderCodeGLcharPtr, nullptr);
    return shaderProgram;
}

GLuint Shader::CreateShaderProgramOfType(ShaderType_ shaderType) {
    switch(shaderType) {
    case ShaderType_Vertex:
        return glCreateShader(GL_VERTEX_SHADER);
    case ShaderType_Fragment:
        return glCreateShader(GL_FRAGMENT_SHADER);
    case ShaderType_Geometry:
        return glCreateShader(GL_GEOMETRY_SHADER);
    }
}

Shader::~Shader() {
    glDeleteProgram(programId);
}

std::string Shader::LoadShaderFile(std::string path) {
    std::string shaderCode;
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {
        shaderFile.open(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    } catch(const std::ifstream::failure &e) {
        std::cerr << "[EXCEPTION: Shader::LoadShaderFile] " << e.what() << '\n' << e.code() << '\n';
        std::cerr << "There was a problem with opening a " << path << " file.\n";
        exit(1);
    }
    return shaderCode;
}

void Shader::CompileShader(GLuint vertexShader, GLuint fragmentShader, GLuint geomShader) {
    GLint result = GL_FALSE;
    int infoLogLength;

    GL_CHECK(glCompileShader(vertexShader));
    GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result));
    GL_CHECK(glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
        GL_CHECK(glGetShaderInfoLog(vertexShader, infoLogLength, nullptr, &vertexShaderErrorMessage[0]));
        std::cerr << "[VERTEX SHADER COMPILATION ERROR] " << &vertexShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    GL_CHECK(glCompileShader(fragmentShader));
    GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result));
    GL_CHECK(glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
        GL_CHECK(glGetShaderInfoLog(fragmentShader, infoLogLength, nullptr, &fragmentShaderErrorMessage[0]));
        std::cerr << "[FRAGMENT SHADER COMPILATION ERROR] " << &fragmentShaderErrorMessage[0] << '\n';
        exit(1);
    } else result = GL_FALSE;

    if(geomShader != 0) {
        GL_CHECK(glCompileShader(geomShader));
        GL_CHECK(glGetShaderiv(geomShader, GL_COMPILE_STATUS, &result));
        GL_CHECK(glGetShaderiv(geomShader, GL_INFO_LOG_LENGTH, &infoLogLength));
        if(infoLogLength > 0 && !result) {
            std::vector<char> geomShaderErrorMessage(infoLogLength + 1);
            GL_CHECK(glGetShaderInfoLog(geomShader, infoLogLength, nullptr, &geomShaderErrorMessage[0]));
            std::cerr << "[GEOMETRY SHADER COMPILATION ERROR] " << &geomShaderErrorMessage[0] << '\n';
            exit(1);
        } else result = GL_FALSE;
    }

    programId = glCreateProgram();
    GL_CHECK(glAttachShader(programId, vertexShader));
    GL_CHECK(glAttachShader(programId, fragmentShader));
    if(geomShader != 0)
        GL_CHECK(glAttachShader(programId, geomShader));        
    
    GL_CHECK(glLinkProgram(programId));
    GL_CHECK(glGetProgramiv(programId, GL_LINK_STATUS, &result));
    GL_CHECK(glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength));
    if(infoLogLength > 0 && !result) {
        std::vector<char> errorMessage(infoLogLength + 1);
        GL_CHECK(glGetProgramInfoLog(programId, infoLogLength, nullptr, &errorMessage[0]));
        std::cerr << "[SHADER LINKING ERROR] " << &errorMessage[0] << '\n';
        exit(1);
    }
}

void Shader::Use() const {
    GL_CHECK(glUseProgram(programId));
}

GLint Shader::GetUniform(const std::string &uniform) const {
    GLint loc = GL_CHECK(glGetUniformLocation(programId, uniform.c_str())) ;
    return loc;
}

void Shader::Uniform3f(const std::string& name, const glm::vec3& v) const {
    Use();
    GL_CHECK(glUniform3f(GetUniform(name), v.x, v.y, v.z));
}

void Shader::Uniform3f(const std::string& name, GLfloat v1, GLfloat v2, GLfloat v3) const {
    Use();
    GL_CHECK(glUniform3f(GetUniform(name), v1, v2, v3));    
} 

void Shader::UniformMatrix4fv(const std::string& name, const glm::mat4& matrix) const {
    Use();
    GL_CHECK(glUniformMatrix4fv(GetUniform(name), 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::Uniform1i(const std::string& name, GLint v) const {
    Use();
    GL_CHECK(glUniform1i(GetUniform(name), v));    
}

void Shader::Uniform1f(const std::string& name, GLfloat v) const {
    Use();
    GL_CHECK(glUniform1f(GetUniform(name), v));    
}

void Shader::Uniform2f(const std::string& name, GLfloat v1, GLfloat v2) const {
    Use();
    GL_CHECK(glUniform2f(GetUniform(name), v1, v2));    
} 
