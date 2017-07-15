#pragma once

#include "drawable.hpp"
#include "window.hpp"
#include "framebuffer.hpp"


class TopViewScreenQuad : public Drawable {
private:
    Framebuffer *fb;

    void SetBuffer();
    void SetBufferData();
    void SetBufferAttributes();
public:
    TopViewScreenQuad(Framebuffer *fb);
    void Draw();
};