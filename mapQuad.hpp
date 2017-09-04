#pragma once

#include "img.hpp"
#include "drawable.hpp"



class MapQuad : public Drawable {     
private:
    GLuint texId;
    GLfloat vertices[30] {
        // vertices  // tex coords
        -1,  1, 0,   0, 0,
        -1, -1, 0,   0, 1,
         1,  1, 0,   1, 0,
         1,  1, 0,   1, 0,
        -1, -1, 0,   0, 1,
         1, -1, 0,   1, 1            
    };
    float imgRatio;
    int imgSizeX, imgSizeY;
    bool isActive = true;

    void PreserveImgRatio() {
        const float screenRatio = (float)Window::width / (float)Window::height;
        float modX = 0, modY = 0;
        if(screenRatio > imgRatio) {
            float yFactor = (float)Window::height / (float)imgSizeY;
            int shrinkedImgX = imgSizeX * yFactor;
            int xDiff = Window::width - shrinkedImgX;
            modX = xDiff / (float)Window::width;
        } else if(imgRatio > screenRatio) {
            float xFactor = (float)Window::width / (float)imgSizeX;
            int shrinkedImgY = imgSizeY * xFactor;
            int yDiff = Window::height - shrinkedImgY;
            modY = yDiff / (float)Window::height;
        }
        vertices[0] = vertices[5] = vertices[20] = -1 + modX;
        vertices[10] = vertices[15] = vertices[25] = 1 - modX;
        vertices[1] = vertices[11] = vertices[16] = 1 - modY;
        vertices[6] = vertices[21] = vertices[26] = -1 + modY;
    }

    void ModifyBuffer() {
        PreserveImgRatio();
        BindVbo();
        GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
        UnbindVbo();
    }

public:
    MapQuad(Img& img) 
    : Drawable("shaders/framebufferVertexShader.glsl", "shaders/framebufferFragmentShader.glsl") {
        imgSizeX = img.GetSizeX();
        imgSizeY = img.GetSizeY(); 
        imgRatio = imgSizeX / imgSizeY;
        PreserveImgRatio();
        BindVbo();
        GL_CHECK(glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            &vertices,
            GL_DYNAMIC_DRAW
        ));
        BindVao();
        GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
        GL_CHECK(glEnableVertexAttribArray(0));
        GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                       (GLvoid*)(3 * sizeof(GLfloat)))); 
        GL_CHECK(glEnableVertexAttribArray(1));
        UnbindVao();
        UnbindVbo();

        GL_CHECK(glGenTextures(1, &texId));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, texId));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img.GetSizeX(), img.GetSizeY(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.GetData()));

        shader.Uniform1i("screenTexture", 4);

        SingletonEvent<FramebufferSizeChangedEvent>::Instance()->Register([this](){
            this->OnFramebufferSizeChange();
        });
    }

    ~MapQuad() {
        glDeleteTextures(1, &texId);
    }

    void Draw() {
        if(!isActive) return;
        shader.Use();
        BindVao();
        GL_CHECK(glActiveTexture(GL_TEXTURE4));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, texId));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
        UnbindVao();
    }

    void OnFramebufferSizeChange() {
        ModifyBuffer();
    }

    void ToggleVisibility() {
        isActive = !isActive;
    }
};