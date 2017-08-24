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

    /* Pobiera jako argumenty typ shadera i ścieżkę, a zwraca identyfikator
       shadera w OpenGL. */
    GLuint GetShaderProgram(ShaderType_ shaderType, const std::string& shaderPath);
    /* Tworzy w OpenGL shader typu 'shaderType' i zwraca jego identyfikator. */
    GLuint CreateShaderProgramOfType(ShaderType_ shaderType);
    /* Otwiera plik pod ścieżką 'shaderPath' i zwraca std::string zawierający kod shadera. */
    std::string LoadShaderFile(std::string shaderPath);
    /* Kompiluje program z dostarczonych identyfikatorów do vertex, fragment i geometry shadera
       i zapisuje jego identyfikator w 'programId'. */
    void CompileShader(GLuint vertexShader, GLuint fragmentShader, GLuint geomShader = 0);
public:
    Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath,
           const std::string& geomShaderPath = "");
    ~Shader();
    void Use() const;

    /* Poniższe metody stanowią opakowanie dla funkcji z OpenGL API służących do wysyłania wartości
       do zmiennych "uniform" w shaderze. */
    GLint GetUniform(const std::string &uniform) const;
    void Uniform3f(const std::string& name, const glm::vec3& v) const;
    void Uniform3f(const std::string& name, GLfloat v1, GLfloat v2, GLfloat v3) const;
    void UniformMatrix4fv(const std::string& name, const glm::mat4& matrix) const;
    void Uniform1i(const std::string& name, GLint v) const;
    void Uniform1f(const std::string& name, GLfloat v) const;
    void Uniform2f(const std::string& name, GLfloat v1, GLfloat v2) const;
};