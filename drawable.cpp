#include "drawable.hpp"
#include "glDebug.hpp"


Drawable::Drawable(std::string vshader, std::string fshader) 
: shader(vshader, fshader) {
    GL_CHECK(glGenVertexArrays(1, &vaoId));
    GL_CHECK(glGenBuffers(1, &vboId));
}

Drawable::~Drawable() {
    GL_CHECK(glDeleteVertexArrays(1, &vaoId));
    GL_CHECK(glDeleteBuffers(1, &vboId));
}

void Drawable::BindVao() {
    GL_CHECK(glBindVertexArray(vaoId));
}

void Drawable::UnbindVao() {
    GL_CHECK(glBindVertexArray(0));
}

void Drawable::BindVbo() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboId));
}

void Drawable::UnbindVbo() {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
}