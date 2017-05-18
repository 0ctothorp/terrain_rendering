#include <iostream>

#include "mouse.hpp"


Mouse::Mouse(double _x, double _y, Camera *_camera) :
lastX(_x),
lastY(_y), 
camera(_camera) {}

void Mouse::MoveCallback(double xpos, double ypos) {
    if(firstTime) {
        lastX = xpos;
        lastY = ypos;    
        firstTime = false;
    }
    camera->ProcessMouseMovement(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}

void Mouse::ScrollCallback(double yoffset) {
    camera->ChangeMovementSpeed(yoffset);
}