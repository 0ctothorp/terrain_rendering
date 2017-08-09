#pragma once

#include <string>

#include <GL/glew.h>


class Shader {
private:
    GLuint programId;
    enum ShaderType_ {
        ShaderType_Vertex,
        ShaderType_Fragment,
        ShaderType_Geometry
    };

    GLuint CreateShaderProgram(ShaderType_ shaderType, const std::string& shaderPath);
    std::string LoadShaderFile(std::string shaderPath);
    void CompileShader(GLuint vertexShader, GLuint fragmentShader);
public:
    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
           const std::string& geomShaderPath = "");
    ~Shader();
    void Use() const;
    GLuint GetUniform(const std::string &uniform) const;
};