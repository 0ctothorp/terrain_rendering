#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Camera {
protected:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:
    static constexpr float fov = 60.f;

    Camera(glm::vec3 _position) 
    : position(_position) {}

    virtual glm::mat4 GetViewMatrix() const = 0;

    void SetPosition(glm::vec3 pos) {
        position = pos;
    }

    glm::vec3 GetPosition() const {
        return position;
    }

    glm::vec3 GetFront() const {
        return front;
    }
};
