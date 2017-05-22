#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


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

class Camera {
private:
    float Yaw;
    float Pitch;
    float movementSpeed;
    float MouseSensitivity;

    void updateCameraVectors() {
        glm::vec3 _front;
        float cosPitch = cos(glm::radians(Pitch));
        _front.x = sin(glm::radians(Yaw)) * cosPitch;
        _front.y = sin(glm::radians(Pitch));
        _front.z = -cos(glm::radians(Yaw)) * cosPitch;
        front = glm::normalize(_front);
        // Normalize the vectors, because their length gets closer to 0
        // the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, WorldUp));
        up    = glm::normalize(glm::cross(right, front));
        leftViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(-fov / 2.f));
        rightViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(fov / 2.f));
    }

public:
    static constexpr float fov = 60.f;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 WorldUp;
    glm::vec2 leftViewLimit;
    glm::vec2 rightViewLimit;

    Camera(glm::vec3 position)
    : front(glm::vec3(0.0f, 0.0f, -1.0f))
    , movementSpeed(SPEED)
    , MouseSensitivity(SENSITIVTY)
    , position(position) {
        WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        right = glm::normalize(glm::cross(front, WorldUp));
        up    = glm::normalize(glm::cross(right, front));
        leftViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(-fov / 2.f));
        rightViewLimit = glm::rotate(glm::vec2(front.x, front.z), glm::radians(fov / 2.f));
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    void Move(bool *keys, double deltaTime) {
        GLfloat velocity = movementSpeed * deltaTime;
        glm::vec3 prevPos = position;

        if(keys[GLFW_KEY_W]) position += front * velocity;
        else if(keys[GLFW_KEY_S]) position -= front * velocity;
        if(keys[GLFW_KEY_A]) position -= right * velocity;
        else if(keys[GLFW_KEY_D]) position += right * velocity;
        if(keys[GLFW_KEY_SPACE]) position += WorldUp * velocity;
        if(keys[GLFW_KEY_LEFT_CONTROL]) position -= WorldUp * velocity;
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        while(Yaw > 360.0f) Yaw -= 360.0f;
        Pitch -= yoffset;
        if(Pitch > 89.0f) Pitch = 89.0f;
        if(Pitch < -89.0f) Pitch = -89.0f;
        updateCameraVectors();
    }

    void setPosition(glm::vec3 pos) {
        position = pos;
    }

    void ChangeMovementSpeed(int change) {
        if((change < 0 && movementSpeed > 1) || change > 0)
            movementSpeed += change;
    }
};