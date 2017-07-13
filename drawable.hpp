#pragma once

#include <GL/glew.h>

#include "glDebug.hpp"
#include "window.hpp"
#include "framebuffer.hpp"

class Drawable {
protected:
    GLuint vaoId, vboId;
    Shader shader;

public:
    Drawable(string vshader, string fshader) 
    : shader(vshader, fshader) {
        GL_CHECK(glGenVertexArrays(1, &vaoId));
        GL_CHECK(glGenBuffers(1, &vboId));
    }

    ~Drawable() {
        GL_CHECK(glDeleteVertexArrays(1, &vaoId));
        GL_CHECK(glDeleteBuffers(1, &vboId));
    }

    void BindVao() {
        GL_CHECK(glBindVertexArray(vaoId));
    }

    void UnbindVao() {
        GL_CHECK(glBindVertexArray(0));
    }

    void BindVbo() {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboId));
    }

    void UnbindVbo() {
        GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};

class TopViewScreenQuad : public Drawable {
private:
    float screenRatio;
    float topViewXsize;
    float topViewYsize;
    Framebuffer *fb;
    GLfloat *topViewQuad;

    void Init() {
        screenRatio = (float)Window::width / (float)Window::height;
        float topViewXsize = 0.6f;
        float topViewYsize = ((topViewXsize * fb->GetResHeight()) / 
                              (float)fb->GetResWidth()) * screenRatio;

        topViewQuad = new GLfloat[30] {
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

    void SetBuffer() {
        BindVbo();
        GL_CHECK(glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(topViewQuad) * sizeof(GLfloat),
            &topViewQuad,
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
public:
    TopViewScreenQuad(string vshader, string fshader, Framebuffer *fb) 
    : Drawable(vshader, fshader)
    , fb(fb) {
        Init();
    }

    ~TopViewScreenQuad() {
        delete[] topViewQuad;
    }

    void Draw() {
        shader.Use();
        GL_CHECK(glDisable(GL_DEPTH_TEST));
        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, fb->GetColorTexture()));
        BindVao();
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
        UnbindVao();
    }
};