#pragma once

#include "camera.hpp"

enum Camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW            =  0.0f;
const GLfloat PITCH          = -25.0f;
const GLfloat SPEED          =  1.0f;
const GLfloat SENSITIVTY     =  0.15f;
const GLfloat COL_OFFSET     =  0.15f;

class MainCamera : public Camera {
private:
    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec2 leftViewLimit;
    glm::vec2 rightViewLimit;

    void updateCameraVectors() {
        glm::vec3 _front;
        float radPitch = glm::radians(pitch);
        float cosPitch = cos(radPitch);
        float radYaw = glm::radians(yaw);
        _front.x = sin(radYaw) * cosPitch;
        _front.y = sin(radPitch);
        _front.z = -cos(radYaw) * cosPitch;
        front = glm::normalize(_front);
        // Normalize the vectors, because their length gets closer to 0
        // the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
        
        leftViewLimit = glm::rotate(glm::normalize(glm::vec2(front.x, front.z)), 
                                    glm::radians(-fov / 2.f));
        rightViewLimit = glm::rotate(glm::normalize(glm::vec2(front.x, front.z)), 
                                     glm::radians(fov / 2.f));
    }

public:
    static constexpr float fov = 60.f;

    MainCamera(glm::vec3 position)
    : Camera(position)
    , movementSpeed(SPEED)
    , mouseSensitivity(SENSITIVTY) {
        worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
        leftViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(-fov / 2.f));
        rightViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(fov / 2.f));
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
    }

    void setPosition(glm::vec3 pos) {
        position = pos;
    }

    void ChangeMovementSpeed(int change) {
        if((change < 0 && movementSpeed > 1) || change > 0)
            movementSpeed += change;
    }

    glm::vec2 GetLeftViewLimit() const {
        return leftViewLimit;
    }

    glm::vec2 GetRightViewLimit() const {
        return rightViewLimit;
    }
};
