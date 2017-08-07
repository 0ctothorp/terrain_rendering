#include <glm/gtc/matrix_transform.hpp>

#include "topCamera.hpp"
#include "window.hpp"


TopCamera::TopCamera(float height) 
: Camera(glm::vec3(0, height, 0))
, projectionMatrix(glm::perspective(glm::radians(60.0f), 
    (float)Window::width / (float)Window::height, 0.1f, 3000.0f)) {
    front = glm::vec3(0.0f, -1.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, -1.0f);
}

glm::mat4 TopCamera::GetViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}