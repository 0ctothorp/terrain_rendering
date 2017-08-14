#pragma once

#include "drawable.hpp"
#include "glDebug.hpp"


class NormalMapQuad : public Drawable {
private:
    GLuint textureId;
public:
    NormalMapQuad(GLuint texId) 
    : Drawable("shaders/framebufferVertexShader.glsl", "shaders/normalQuadFragment.glsl")
    , textureId(texId) {
        BindVbo();
        SetBufferData();
        SetBufferAttributes();
        UnbindVbo();
        shader.Uniform1i("screenTexture", 2);
    }

    void SetBufferData() {
        float screenRatio = (float)Window::width / (float)Window::height;
        float xsize = 0.6f;
        float ysize = xsize * screenRatio;
        float minus = 0.65f;
        GLfloat viewQuad[] {
            -1.0f, 1 - ysize - minus, 0.0f, 0.0f, 0.0f,
            -1.0f, 1.0f - minus, 0.0f, 0.0f, 1.0f,
            -1 + xsize, 1.0f - minus, 0.0f, 1.0f, 1.0f,
            -1 + xsize, 1.0f - minus, 0.0f, 1.0f, 1.0f,
            -1 + xsize, 1 - ysize - minus, 0.0f, 1.0f, 0.0f,
            -1.0f, 1 - ysize - minus, 0.0f, 0.0f, 0.0f
        };
        GL_CHECK(glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(viewQuad) * sizeof(GLfloat),
            &viewQuad,
            GL_STATIC_DRAW
        ));
    }

    void SetBufferAttributes() {
        BindVao();
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                    (GLvoid*)(3 * sizeof(GLfloat)))); 
        GL_CHECK(glEnableVertexAttribArray(1));
        UnbindVao();
    }

    void Draw() {
        shader.Use();
        // GL_CHECK(glDisable(GL_DEPTH_TEST));
        BindVao();
        GL_CHECK(glActiveTexture(GL_TEXTURE2));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, textureId));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
        UnbindVao();
    }

};