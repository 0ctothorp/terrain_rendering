#include "topViewScreenQuad.hpp"
#include "glDebug.hpp"


TopViewScreenQuad::TopViewScreenQuad(Framebuffer *fb) 
: Drawable("shaders/framebufferVertexShader.glsl", "shaders/framebufferFragmentShader.glsl")
, fb(fb) {
    SetBuffer();
    shader.Use();
    GL_CHECK(glUniform1i(shader.GetUniform("screenTexture"), 1));
}

void TopViewScreenQuad::SetBuffer() {
    BindVbo();
    SetBufferData();
    SetBufferAttributes();
    UnbindVbo();
}

void TopViewScreenQuad::SetBufferData() {
    float screenRatio = (float)Window::width / (float)Window::height;
    float topViewXsize = 0.6f;
    float topViewYsize = ((topViewXsize * fb->GetResHeight()) / 
                         (float)fb->GetResWidth()) * screenRatio;
    GLfloat topViewQuad[] {
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1 - topViewYsize, 0.0f, 1.0f, 0.0f,
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f
    };
    GL_CHECK(glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(topViewQuad) * sizeof(GLfloat),
        &topViewQuad,
        GL_STATIC_DRAW
    ));
}

void TopViewScreenQuad::SetBufferAttributes() {
    BindVao();
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                   (GLvoid*)(3 * sizeof(GLfloat)))); 
    GL_CHECK(glEnableVertexAttribArray(1));
    UnbindVao();
}

void TopViewScreenQuad::Draw() {
    shader.Use();
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    BindVao();
    GL_CHECK(glActiveTexture(GL_TEXTURE1));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, fb->GetColorTexture()));
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
    UnbindVao();
}