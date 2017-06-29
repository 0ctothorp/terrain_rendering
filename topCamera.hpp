#pragma once

#include "camera.hpp"

class TopCamera : public Camera {
public:
    TopCamera(float height) : Camera(glm::vec3(0, height, 0)) {
        front = glm::vec3(0.0f, -1.0f, 0.0f);
        up = glm::vec3(0.0f, 0.0f, -1.0f);
    }

    virtual glm::mat4 GetViewMatrix() const override {
        return glm::lookAt(position, position + front, up);
    }
};