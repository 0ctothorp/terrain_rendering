#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>


class Shader {
private:
    GLuint programId;
    enum ShaderType_ {
        ShaderType_Vertex,
        ShaderType_Fragment,
        ShaderType_Geometry
    };

    GLuint GetShaderProgram(ShaderType_ shaderType, const std::string& shaderPath);
    GLuint CreateShaderProgramOfType(ShaderType_ shaderType);
    std::string LoadShaderFile(std::string shaderPath);
    void CompileShader(GLuint vertexShader, GLuint fragmentShader, GLuint geomShader = 0);
public:
    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
           const std::string& geomShaderPath = "");
    ~Shader();
    void Use() const;
    GLint GetUniform(const std::string &uniform) const;
    void Uniform3f(const std::string& name, const glm::vec3& v) const;
    void Uniform3f(const std::string& name, GLfloat v1, GLfloat v2, GLfloat v3) const;
    void UniformMatrix4fv(const std::string& name, const glm::mat4& matrix) const;
    void Uniform1i(const std::string& name, GLint v) const;
    void Uniform1f(const std::string& name, GLfloat v) const;
    void Uniform2f(const std::string& name, GLfloat v1, GLfloat v2) const;
};