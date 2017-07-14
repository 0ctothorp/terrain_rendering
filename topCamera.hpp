#pragma once

#include "camera.hpp"


class TopCamera : public Camera {
public:
    const glm::mat4x4 projectionMatrix;
    TopCamera(float height);
    virtual glm::mat4 GetViewMatrix() const override;
};