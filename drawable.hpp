#pragma once

#include <string>

#include <GL/glew.h>

#include "shader.hpp"


class Drawable {
protected:
    GLuint vaoId, vboId;
    Shader shader;

public:
    Drawable(const std::string& vshader, const std::string& fshader, 
             const std::string& geomShader = "");
    ~Drawable();
    void BindVao();
    void UnbindVao();
    void BindVbo();
    void UnbindVbo();
};