#include <iostream>

#include "framebuffer.hpp"
#include "glDebug.hpp"


Framebuffer::Framebuffer() {
    GL_CHECK(glGenFramebuffers(1, &framebuffer));
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    CreateColorTexture();
    CreateRenderBuffer();
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::~Framebuffer() {
    GL_CHECK(glDeleteFramebuffers(1, &framebuffer));
    GL_CHECK(glDeleteTextures(1, &colorTexture));
    GL_CHECK(glDeleteRenderbuffers(1, &renderbuffer));
}

void Framebuffer::CreateColorTexture() {
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, resolutionWidth, resolutionHeight, 0, GL_RGB, 
                 GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                           colorTexture, 0);  
}

void Framebuffer::CreateRenderBuffer() {
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, resolutionWidth, 
                          resolutionHeight);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 
                              renderbuffer);
}

void Framebuffer::Bind() {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
}

void Framebuffer::Unbind() {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}