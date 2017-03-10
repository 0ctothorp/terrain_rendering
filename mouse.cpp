#include <iostream>

#include "mouse.hpp"

using namespace std;

Mouse::Mouse(double _x, double _y, Camera *_camera) :
lastX(_x),
lastY(_y), 
camera(_camera) {}

void Mouse::MoveCallback(GLFWwindow*, double xpos, double ypos) {
    if(firstTime) {
        lastX = xpos;
        lastY = ypos;    
        firstTime = false;
    }
    camera->ProcessMouseMovement(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}
