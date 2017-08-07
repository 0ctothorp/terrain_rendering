#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "window.hpp"
#include "tileMesh.hpp"
#include "frustum.hpp"


class MainCamera : public Camera {
private:
    Frustum frustum;
    float yaw = glm::radians(0.0f);
    float pitch = glm::radians(-25.0f);
    float movementSpeed = 1.0f;
    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 position2;
    bool meshMovementLocked = false;

    void updateCameraVectors();
public:
    const float fov = 60.f;
    const float near = 0.01f;
    const float far = 2000.0f;
    const glm::mat4x4 projectionMat = glm::perspective(
        glm::radians(fov), 
        (float)Window::width / (float)Window::height,
        near,
        far
    );

    MainCamera(glm::vec3 position = glm::vec3(0, 20, 0));
    virtual glm::mat4 GetViewMatrix() const override;
    void Move(bool *keys, double deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, float sensitivity);
    void ChangeMovementSpeed(int change);
    bool IsInsideFrustum(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, 
                         const glm::vec3 &point4) const;
    void ToggleMeshMovementLock();
};
