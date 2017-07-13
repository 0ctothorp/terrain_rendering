#include "topViewScreenQuad.hpp"
#include "glDebug.hpp"


TopViewScreenQuad::TopViewScreenQuad(std::string vshader, std::string fshader, Framebuffer *fb) 
: Drawable(vshader, fshader)
, fb(fb) {
    Init();
}

TopViewScreenQuad::~TopViewScreenQuad() {
    delete[] topViewQuad;
}

void TopViewScreenQuad::Init() {
    screenRatio = (float)Window::width / (float)Window::height;
    float topViewXsize = 0.6f;
    float topViewYsize = ((topViewXsize * fb->GetResHeight()) / 
                          (float)fb->GetResWidth()) * screenRatio;

    topViewQuad = new GLfloat[bufferSize] {
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1 - topViewYsize, 0.0f, 1.0f, 0.0f,
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f
    };

    SetBuffer();
    shader.Use();
    GL_CHECK(glUniform1i(shader.GetUniform("screenTexture"), 1));
}

void TopViewScreenQuad::SetBuffer() {
    BindVbo();
    GL_CHECK(glBufferData(
        GL_ARRAY_BUFFER,
        bufferSize * sizeof(GLfloat),
        topViewQuad,
        GL_STATIC_DRAW
    ));
    BindVao();
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                   (GLvoid*)(3 * sizeof(GLfloat)))); 
    GL_CHECK(glEnableVertexAttribArray(1));
    UnbindVao();
    UnbindVbo();
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