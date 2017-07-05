#include <iostream>

#include "framebuffer.hpp"
#include "glDebug.hpp"


Framebuffer::Framebuffer(int texResWidth, int texResHeight) {
    resolutionWidth = texResWidth;
    resolutionHeight = texResHeight;
    GL_CHECK(glGenFramebuffers(1, &framebuffer));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    CreateColorTexture();
    CreateRenderBuffer();
    GLint status = GL_CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER));
    if(status != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Framebuffer::~Framebuffer() {
    GL_CHECK(glDeleteFramebuffers(1, &framebuffer));
    GL_CHECK(glDeleteTextures(1, &colorTexture));
    GL_CHECK(glDeleteRenderbuffers(1, &renderbuffer));
}

void Framebuffer::CreateColorTexture() {
    GL_CHECK(glGenTextures(1, &colorTexture));
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, colorTexture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolutionWidth, resolutionHeight, 0, GL_RGB, 
                          GL_UNSIGNED_BYTE, nullptr));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));

    GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                                    colorTexture, 0));  
}

void Framebuffer::CreateRenderBuffer() {
    GL_CHECK(glGenRenderbuffers(1, &renderbuffer));
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer)); 
    GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolutionWidth, 
                                   resolutionHeight));  
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 
                                       renderbuffer));
}

void Framebuffer::Bind() const {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
}

void Framebuffer::Unbind() const {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

GLuint Framebuffer::GetColorTexture() const {
    return colorTexture;
}

int Framebuffer::GetResWidth() const {
    return resolutionWidth;
}

int Framebuffer::GetResHeight() const {
    return resolutionHeight;
}