#pragma once

#include <vector>
#include <memory>
#include <iostream> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

using namespace std;

enum Camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const GLfloat YAW        =  45.0f;
const GLfloat PITCH      =  -25.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.15f;
const GLfloat COL_OFFSET =  0.15f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    GLfloat Yaw;
    GLfloat Pitch;

    GLfloat MovementSpeed;
    GLfloat MouseSensitivity;

    // @TODO: Make constructor that takes lookAt vector.
    Camera(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f))
    : Front(glm::vec3(0.0f, 0.0f, 1.0f))
    , MovementSpeed(SPEED)
    , MouseSensitivity(SENSITIVTY) {
        Position = position;
        WorldUp = glm::vec3(0, 1, 0);
        Yaw = YAW;
        Pitch = PITCH;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    int i = 0;
    void ProcessKeyboard(Camera_movement direction, GLfloat deltaTime) {
        GLfloat velocity = MovementSpeed * deltaTime;

        if (direction == FORWARD)
            Position += glm::normalize(glm::vec3(Front.x, Front.y, Front.z)) * 
                        velocity;
        else if (direction == BACKWARD)
            Position -= glm::normalize(glm::vec3(Front.x, Front.y, Front.z)) * 
                        velocity;

        if (direction == LEFT)
            Position -= Right * velocity;
        else if (direction == RIGHT)
            Position += Right * velocity;
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset,
                              GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        // @TODO: Option to invert Y axis.
        Yaw   += xoffset;
        Pitch -= yoffset;

        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void setPosition(glm::vec3 pos) {
        Position = pos;
    }

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors() {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        // Normalize the vectors, because their length gets closer to 0
        // the more you look up or down which results in slower movement.
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};