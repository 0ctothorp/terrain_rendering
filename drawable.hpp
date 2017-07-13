#pragma once

#include <string>

#include <GL/glew.h>

#include "shader.hpp"


class Drawable {
protected:
    GLuint vaoId, vboId;
    Shader shader;

public:
    Drawable(std::string vshader, std::string fshader);
    ~Drawable();
    void BindVao();
    void UnbindVao();
    void BindVbo();
    void UnbindVbo();
};