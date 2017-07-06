#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Mouse {
private:
    static Mouse *instance;
    double lastX;
    double lastY;
    bool firstTime = true;

    Mouse(double _x, double _y);
    ~Mouse();
public:
    float sensitivity = 0.15f;

    void MoveCallback(double xpos, double ypos);
    void ScrollCallback(double yoffset);
    static Mouse* GetInstance();
};