#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Mouse {
private:
    double lastX;
    double lastY;
    bool firstTime = true;
    MainCamera* mainCam;
public:
    float sensitivity = 0.15f;

    Mouse(double _x, double _y, MainCamera* mainCam);
    void MoveCallback(double xpos, double ypos);
    void ScrollCallback(double yoffset);
};