#pragma once

#include <string>

#include <GL/glew.h>

using namespace std;

class Shader {
private:
    GLuint programId;

    void CompileShader(GLuint vertexShader, GLuint fragmentShader);
    string LoadShaderFile(string shaderPath);
public:
    Shader(const string &vertexShaderPath, const string &fragmentShaderPath);
    ~Shader();
    GLuint GetProgramId() const;
};