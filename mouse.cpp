#include <iostream>

#include "window.hpp"
#include "mainCamera.hpp"
#include "mouse.hpp"


Mouse* Mouse::instance = nullptr;

Mouse::Mouse(double _x, double _y) : lastX(_x), lastY(_y) {}

Mouse::~Mouse() {
    delete instance;
}

void Mouse::MoveCallback(double xpos, double ypos) {
    if(firstTime) {
        lastX = xpos;
        lastY = ypos;    
        firstTime = false;
    }
    MainCamera::GetInstance()->ProcessMouseMovement(xpos - lastX, ypos - lastY);
    lastX = xpos;
    lastY = ypos;
}

void Mouse::ScrollCallback(double yoffset) {
    MainCamera::GetInstance()->ChangeMovementSpeed(yoffset);
}

Mouse* Mouse::GetInstance() {
    if(instance == nullptr)
        instance = new Mouse(Window::width / 2.0f, Window::height / 2.0f);
    return instance;
}
