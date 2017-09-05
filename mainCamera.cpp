#include <iostream>

#include "mainCamera.hpp"
#include "mouse.hpp"


MainCamera::MainCamera(glm::vec3 position)
: Camera(position)
, frustum(this, &projectionMat) 
, position2(position) 
, prevPosition(position){
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
    frustum.ExtractPlanes();
}

void MainCamera::updateCameraVectors() {
    float radPitch = glm::radians(pitch);
    float cosPitch = cos(radPitch);
    float radYaw = glm::radians(yaw);
    glm::vec3 _front(sin(radYaw) * cosPitch, sin(radPitch), -cos(radYaw) * cosPitch);
    front = glm::normalize(_front);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

glm::mat4 MainCamera::GetViewMatrix() const {
    if(!meshMovementLocked)
        return glm::lookAt(position, position + front, up);
    else 
        return glm::lookAt(position2, position2 + front, up);
}

void MainCamera::Move(bool *keys, double deltaTime) {
    GLfloat velocity = movementSpeed * deltaTime;
    if(!meshMovementLocked) {
        if(keys[GLFW_KEY_W]) position += front * velocity;
        else if(keys[GLFW_KEY_S]) position -= front * velocity;
        if(keys[GLFW_KEY_A]) position -= right * velocity;
        else if(keys[GLFW_KEY_D]) position += right * velocity;
        if(keys[GLFW_KEY_SPACE]) position += worldUp * velocity;
        if(keys[GLFW_KEY_LEFT_CONTROL]) position -= worldUp * velocity;
    
        frustum.ExtractPlanes();
    } else {
        if(keys[GLFW_KEY_W]) position2 += front * velocity;
        else if(keys[GLFW_KEY_S]) position2 -= front * velocity;
        if(keys[GLFW_KEY_A]) position2 -= right * velocity;
        else if(keys[GLFW_KEY_D]) position2 += right * velocity;
        if(keys[GLFW_KEY_SPACE]) position2 += worldUp * velocity;
        if(keys[GLFW_KEY_LEFT_CONTROL]) position2 -= worldUp * velocity;
    }
    if(position != prevPosition) {
        lat = lat - ((position.z - prevPosition.z) / 1200.0f);
        lon = lon + ((position.x - prevPosition.x) / 1200.0f);
    }
    prevPosition = position;
}

void MainCamera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, float sensitivity) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    while(yaw > 360.0f) yaw -= 360.0f;

    pitch -= yoffset;
    if(pitch > 89.0f) pitch = 89.0f;
    if(pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();

    if(!meshMovementLocked) 
        frustum.ExtractPlanes();
}

void MainCamera::ChangeMovementSpeed(int change) {
    if((change < 0 && movementSpeed > 1) || change > 0)
        movementSpeed += change;
}

bool MainCamera::IsInsideFrustum(const glm::vec3 &point1, const glm::vec3 &point2, 
                                 const glm::vec3 &point3, const glm::vec3 &point4) const {
    const glm::vec3 yoff(0, 100, 0);
    const glm::vec3 point12 = point1 + yoff;
    const glm::vec3 point22 = point2 + yoff;
    const glm::vec3 point32 = point3 + yoff;
    const glm::vec3 point42 = point4 + yoff;
    std::array<glm::vec3, 8> points{point1, point2, point3, point4,
                                    point12, point22, point32, point42};
    return frustum.IsCubeInside(points);
}

void MainCamera::SetMeshMovementLock(bool locked) {
    meshMovementLocked = locked;
    if(meshMovementLocked)
        position2 = position;
    else
        position = position2;
}
