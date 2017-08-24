#pragma once

#include "drawable.hpp"
#include "window.hpp"
#include "framebuffer.hpp"


// Quad, na który nałożona będzie tekstura bufora ramki dla widoku z góry.
class TopViewScreenQuad : public Drawable {
private:
    // Wkaźnik na bufor ramki, który ma być użyty do narysowania quada.
    Framebuffer *fb;

    // Metody używane przez konstruktor. Wypełniają bufor wierzchołków danymi i ustawiają atrybuty.
    void SetBuffer();
    void SetBufferData();
    void SetBufferAttributes();
public:
    TopViewScreenQuad(Framebuffer *fb);
    void Draw();
};