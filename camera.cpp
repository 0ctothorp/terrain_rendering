#include "camera.hpp"

Camera::Camera(glm::vec3 _position) : position(_position) {}

void Camera::SetPosition(glm::vec3 pos) {
    position = pos;
}

glm::vec3 Camera::GetPosition() const {
    return position;
}

glm::vec3 Camera::GetFront() const {
    return front;
}