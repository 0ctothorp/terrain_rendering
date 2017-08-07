#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "frustum.hpp"


int Frustum::SignedDistToPoint(int plane, const glm::vec3 &point) const {
    return planes[plane].x * point.x + planes[plane].y * point.y + 
           planes[plane].z * point.z + planes[plane].w;
}

Frustum::Frustum(const Camera* camera, const glm::mat4x4* projMat)
: projMat(projMat)
, camera(camera) {
    ExtractPlanes();
}

void Frustum::ExtractPlanes() {
    glm::mat4x4 model;
    glm::vec3 camPos = camera->GetPosition();
    glm::translate(model, camPos);
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

bool Frustum::IsCubeInside(const std::array<glm::vec3, 8> &points) const {
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