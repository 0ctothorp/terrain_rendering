#include <iostream>


void countFrames(int &frames, double currentFrame, double lastFrame) {
    static double timePassed = 0;
    timePassed += currentFrame - lastFrame;
    if(timePassed >= 1) {
        std::cout << frames << " fps\n";
        frames = 0;
        timePassed = 0;
    }
    frames++;
}