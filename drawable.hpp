#pragma once

#include <string>

#include <GL/glew.h>

#include "shader.hpp"


/* Klasa mająca służyć za bazę do tworzenia innych klas, które mają byc użyte do przedstawiania
   obiektów, które można narysować na ekranie. */
class Drawable {
protected:
    // Vertex array object, vertex buffer object.
    GLuint vaoId, vboId;
    Shader shader;

public:
    /* 'vshader' - ścieżka do vertex shadera.
       'fshader' - ścieżka do fragment shadera.
       'geomShader' - ścieżka do geometry shadera. */
    Drawable(const std::string& vshader, const std::string& fshader, 
             const std::string& geomShader = "");
    ~Drawable();
    void BindVao();
    void UnbindVao();
    void BindVbo();
    void UnbindVbo();
};