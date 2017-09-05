#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "mainCamera.hpp"


// Klasa do obsługi wejścia z myszy komputera.
class Mouse {
private:
    // Ostania pozycja na osi X.
    double lastX;
    // Ostatnia pozycja na osi Y.
    double lastY;
    // Czy nie wykonano jeszcze ruchu myszą?
    bool firstTime = true;
    MainCamera* mainCam;
public:
    // Czułość myszy.
    float sensitivity = 0.15f;

    /* '_x' - startowa pozycja kursora na osi X.
       '_y' - jw., ale na osi Y. */ 
    Mouse(double _x, double _y, MainCamera* mainCam);
    /* Reakcja na ruch myszy - ustawia 'lastX' oraz 'lastY' i informuje 
       główną kamerę 'mainCam' o ruchu. */
    void MoveCallback(double xpos, double ypos, bool just_unlocked = false);
    // Reakcja na użycie "scrolla" - informuje 'mainCam' o zdarzeniu.
    void ScrollCallback(double yoffset);
};