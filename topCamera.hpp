#pragma once

#include "camera.hpp"


/* Kamera z widokiem na teren od góry. Używana do obserwacji działania frustum
   cullingu. */
class TopCamera : public Camera {
public:
    const glm::mat4x4 projectionMatrix;
    // 'height' - wysokość na jakiej ma się znaleźć kamera.
    TopCamera(float height);
    virtual glm::mat4 GetViewMatrix() const override;
};