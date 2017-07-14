#pragma once

#include "framebuffer.hpp"
#include "lodPlane.hpp"
#include "topCamera.hpp"

class TopViewFb : public Framebuffer {
private:
    glm::mat4x4 projectionMatrix;
public:
    TopViewFb(int resWidth, int resHeight);
    void Draw(const LODPlane &lodPlane, TopCamera *topCam);
};