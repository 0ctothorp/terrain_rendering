#pragma once

#include <array>

#include <glm/gtc/matrix_access.hpp>

#include "camera.hpp"
#include "window.hpp"
#include "tileMesh.hpp"

class MainCamera : public Camera {
private:
    float yaw = glm::radians(0.0f);
    float pitch = glm::radians(-25.0f);
    float movementSpeed = 1.0f;
    float mouseSensitivity = 0.15f;
    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);

    class Frustum {
    private: 
        std::array<glm::vec4, 6> planes;
        const glm::mat4x4 *projMat;
        enum PlaneSide {left = 0, right, top, bottom, near, far};

        int SignedDistToPoint(int plane, const glm::vec3 &point) const {
            return planes[plane].x * point.x + planes[plane].y * point.y + 
                   planes[plane].z * point.z + planes[plane].w;
        }

    public:
        Frustum(const Camera* camera, const glm::mat4x4* projMat)
        : projMat(projMat) {
            ExtractPlanes(camera);
        }

        void ExtractPlanes(const Camera* camera) {
            glm::mat4x4 model;
            glm::translate(model, camera->GetPosition());
            glm::mat4x4 vpMat = *projMat * camera->GetViewMatrix() * model;

            glm::vec4 row4 = glm::row(vpMat, 3);
            glm::vec4 row3 = glm::row(vpMat, 2);
            glm::vec4 row2 = glm::row(vpMat, 1);
            glm::vec4 row1 = glm::row(vpMat, 0);

            planes[left] = row4 + row1;
            planes[right] = row4 - row1;
            planes[bottom] = row4 + row2;
            planes[top] = row4 - row2;
            planes[near] = row4 + row3;
            planes[far] = row4 - row3;
        }

        bool IsCubeInside(const std::array<glm::vec3, 8> &points) const {
            for(int i = 0; i < 6; i++) {
                int out = 0;
                for(int j = 0; j < 8; j++) {
                    if(SignedDistToPoint(i, points[j]) < 0)
                        out++;
                }
                if(out == 8)
                    return false;
            }
            return true;
        }
    } frustum;

    void updateCameraVectors() {
        float radPitch = glm::radians(pitch);
        float cosPitch = cos(radPitch);
        float radYaw = glm::radians(yaw);
        glm::vec3 _front(sin(radYaw) * cosPitch, sin(radPitch), -cos(radYaw) * cosPitch);
        front = glm::normalize(_front);
        // Normalize the vectors, because their length gets closer to 0
        // the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

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

    MainCamera(glm::vec3 position)
    : Camera(position)
    , frustum(this, &projectionMat) {
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

    virtual glm::mat4 GetViewMatrix() const override {
        return glm::lookAt(position, position + front, up);
    }

    void Move(bool *keys, double deltaTime) {
        GLfloat velocity = movementSpeed * deltaTime;
        glm::vec3 prevPos = position;

        if(keys[GLFW_KEY_W]) position += front * velocity;
        else if(keys[GLFW_KEY_S]) position -= front * velocity;
        if(keys[GLFW_KEY_A]) position -= right * velocity;
        else if(keys[GLFW_KEY_D]) position += right * velocity;
        if(keys[GLFW_KEY_SPACE]) position += worldUp * velocity;
        if(keys[GLFW_KEY_LEFT_CONTROL]) position -= worldUp * velocity;

        updateCameraVectors();
        frustum.ExtractPlanes(this);
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        while(yaw > 360.0f) yaw -= 360.0f;

        pitch -= yoffset;
        if(pitch > 89.0f) pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;

        updateCameraVectors();
        frustum.ExtractPlanes(this);
    }

    void setPosition(glm::vec3 pos) {
        position = pos;
    }

    void ChangeMovementSpeed(int change) {
        if((change < 0 && movementSpeed > 1) || change > 0)
            movementSpeed += change;
    }

    // Należy sprawdzić kostkę, a nie tylko prostokąt, bo teren znajduje się na wysokości
    // większej niż 0 i podcas poruszania kamerą w górę obcinane jest to co nie powinno.
    bool IsInsideFrustum(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, 
                         const glm::vec3 &point4) const {
        const glm::vec3 y100(0, 100, 0);
        const glm::vec3 point12 = point1 + y100;
        const glm::vec3 point22 = point2 + y100;
        const glm::vec3 point32 = point3 + y100;
        const glm::vec3 point42 = point4 + y100;
        std::array<glm::vec3, 8> points{point1, point2, point3, point4,
                                        point12, point22, point32, point42};
        return frustum.IsCubeInside(points);
    }
};
