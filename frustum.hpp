#pragma once

#include <array>
#include <glm/glm.hpp>

#include "camera.hpp"


class Frustum {
private: 
    std::array<glm::vec4, 6> planes;
    const glm::mat4x4 *projMat;
    const Camera *camera;
    enum Side {left = 0, right, top, bottom, near, far};

    int SignedDistToPoint(int plane, const glm::vec3 &point) const;
public:
    Frustum(const Camera* camera, const glm::mat4x4* projMat);
    void ExtractPlanes();
    bool IsCubeInside(const std::array<glm::vec3, 8> &points) const;
};