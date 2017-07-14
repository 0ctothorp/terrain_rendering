#pragma once

#include "framebuffer.hpp"
#include "lodPlane.hpp"
#include "topCamera.hpp"

class TopViewFb : public Framebuffer {
public:
    TopViewFb(int resWidth, int resHeight);
    void Draw(const LODPlane &lodPlane, TopCamera* topCam, MainCamera* mainCam);
};