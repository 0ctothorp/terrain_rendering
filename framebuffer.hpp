#pragma once

#include <GL/glew.h>


class Framebuffer {
private:
    const int resolutionWidth = 640;
    const int resolutionHeight = 480;
    GLuint framebuffer;
    GLuint renderbuffer;
    GLuint colorTexture;

    void CreateColorTexture();   
    void CreateRenderBuffer(); 

public:
    Framebuffer();
    ~Framebuffer();
    void Bind();
    void Unbind();
};