#pragma once

#include <utility>

// @Note: Has to be included before GLFW because camera includes GLEW.
#include "mainCamera.hpp"

#include <GLFW/glfw3.h>


class Mouse {
private:
    double lastX;
    double lastY;
    float sensitivity = 0.15;
    bool firstTime = true;
    
    MainCamera *camera;
public:
    Mouse(double _x, double _y, MainCamera* camera);
    void MoveCallback(double xpos, double ypos);
    void ScrollCallback(double yoffset);
};