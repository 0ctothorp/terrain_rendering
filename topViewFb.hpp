#pragma once

#include "framebuffer.hpp"
#include "lodPlane.hpp"
#include "topCamera.hpp"


class TopViewFb : public Framebuffer {
private:
    glm::mat4 topViewProjMat;

public:
    TopViewFb(int resWidth, int resHeight);
    void Draw(const LODPlane &lodPlane, TopCamera &topCam) const;
};