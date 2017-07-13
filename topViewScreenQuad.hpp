#pragma once

#include "drawable.hpp"
#include "window.hpp"
#include "framebuffer.hpp"


class TopViewScreenQuad : public Drawable {
private:
    const int bufferSize = 30;
    float screenRatio;
    float topViewXsize;
    float topViewYsize;
    Framebuffer *fb;
    GLfloat *topViewQuad;

    void Init();
    void SetBuffer();
public:
    TopViewScreenQuad(std::string vshader, std::string fshader, Framebuffer *fb);
    ~TopViewScreenQuad();
    void Draw();
};