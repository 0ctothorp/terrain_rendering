#pragma once

#include <GL/glew.h>


class Framebuffer {
private:
    GLuint framebuffer;
    GLuint renderbuffer;
    GLuint colorTexture;
    int resolutionWidth;
    int resolutionHeight;

    void CreateColorTexture();   
    void CreateRenderBuffer(); 

public:
    Framebuffer(int texResWidth, int texResHeight);
    ~Framebuffer();
    void Bind() const;
    void Unbind() const;
    GLuint GetColorTexture() const;
    int GetResWidth() const;
    int GetResHeight() const;
};