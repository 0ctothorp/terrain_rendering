#pragma once

#include <string>

#include <GL/glew.h>


class Shader {
private:
    GLuint programId;

    void CompileShader(GLuint vertexShader, GLuint fragmentShader);
    std::string LoadShaderFile(std::string shaderPath);
public:
    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
    ~Shader();
    void Use() const;
    GLuint GetUniform(const std::string &uniform) const;
};