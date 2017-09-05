#include <iostream>

#include "window.hpp"
#include "mainCamera.hpp"
#include "mouse.hpp"


Mouse::Mouse(double _x, double _y, MainCamera* mainCamera) 
: lastX(_x)
, lastY(_y)
, mainCam(mainCamera) {}

void Mouse::MoveCallback(double xpos, double ypos, bool just_unlocked) {
    if(firstTime || just_unlocked) {
        lastX = xpos;
        lastY = ypos;    
        firstTime = false;
    }
    mainCam->ProcessMouseMovement(xpos - lastX, ypos - lastY, sensitivity);
    lastX = xpos;
    lastY = ypos;
}

void Mouse::ScrollCallback(double yoffset) {
    mainCam->ChangeMovementSpeed(yoffset);
}
