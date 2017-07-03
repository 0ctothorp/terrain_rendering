#pragma once

#include <array>

#include <glm/gtc/matrix_access.hpp>

#include "camera.hpp"
#include "window.hpp"
#include "tileMesh.hpp"


class MainCamera : public Camera {
private:
    class Frustum {
    private: 
        std::array<glm::vec4, 6> planes;
        const glm::mat4x4 *projMat;
        enum Side {left = 0, right, top, bottom, near, far};

        int SignedDistToPoint(int plane, const glm::vec3 &point) const;
    public:
        Frustum(const Camera* camera, const glm::mat4x4* projMat);
        void ExtractPlanes(const Camera* camera);
        bool IsCubeInside(const std::array<glm::vec3, 8> &points) const;
    } frustum;

    static MainCamera *instance;
    float yaw = glm::radians(0.0f);
    float pitch = glm::radians(-25.0f);
    float movementSpeed = 1.0f;
    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);

    MainCamera(glm::vec3 position = glm::vec3(0, 20, 0));
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

    virtual glm::mat4 GetViewMatrix() const override;
    void Move(bool *keys, double deltaTime);
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset);
    void ChangeMovementSpeed(int change);
    bool IsInsideFrustum(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, 
                         const glm::vec3 &point4) const;
    static MainCamera* GetInstance();
};
