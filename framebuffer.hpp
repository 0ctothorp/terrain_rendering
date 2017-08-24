#pragma once

#include <GL/glew.h>


/* Klasa do tworzenia framebufferów. */
class Framebuffer {
protected:
    GLuint framebuffer;
    GLuint renderbuffer;
    GLuint colorTexture;
    int resolutionWidth;
    int resolutionHeight;

    void CreateColorTexture();   
    void CreateRenderBuffer(); 

public:
    /* Argumenty konstruktora to rozdzielczość tekstury, do której rysuje framebufer. */
    Framebuffer(int texResWidth, int texResHeight);
    ~Framebuffer();
    void Bind() const;
    void Unbind() const;
    GLuint GetColorTexture() const;
    int GetResWidth() const;
    int GetResHeight() const;
};
