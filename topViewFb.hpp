#pragma once

#include "framebuffer.hpp"
#include "lodPlane.hpp"
#include "topCamera.hpp"


// Framebuffer dla rednerowania dodatkowego widoku z góry.
class TopViewFb : public Framebuffer {
public:
    // Argumenty to rozdzielczość z jaką ma być renderowany obraz framebuffera
    TopViewFb(int resWidth, int resHeight);
    /* 'lodPlane' - teren, który chcemy oglądać z góry.
       'topCam' - za pomocą jakiej kamery ma być renderowany widok.
       'mainCam' - jaka główna kamera patrzy na 'lodPlane'. Przekazywane po to
           żeby móc pokazać, jak działa drustum culling. */
    void Draw(const LODPlane &lodPlane, TopCamera* topCam, MainCamera* mainCam);
};