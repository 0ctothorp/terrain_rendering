#include <iostream>
#include <vector>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "plane.hpp"
#include "glDebug.hpp"
#include "mouse.hpp"

using namespace std;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

// @Refactor: move this 2 declarations from global to local scope.
Camera camera(glm::vec3(0, 1, 0));
Mouse mouse(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, &camera);

constexpr int MAX_KEY_CODE = 348;
bool keys[MAX_KEY_CODE]{false};

void KeyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mode*/) {
    if(key == -1) {
        cerr << "Pressed unknown key\n";
        return;
    }

    if(action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GL_TRUE);
        return;
    }

    if(action == GLFW_PRESS) {
        keys[key] = true;
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }
}

void GlfwErrorCallback(int /*error*/, const char* description) {
    cerr << "[GLFW ERROR] " << description << '\n';
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    mouse.MoveCallback(window, xpos, ypos);
}

GLFWwindow* GetGLFWwindow(int &w, int &h, const char *name){
    GLFWwindow* window;
    if(!glfwInit()) {
        cerr << "Failed to initialize GLFW\n";
        exit(-1);
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (w == 0 || h == 0) {
        // if 0 then WINDOWLESS FULL SCREEN :
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
        w=mode->width; h=mode->height;
    } else {
        window = glfwCreateWindow(w, h, name, nullptr, nullptr);
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(window == nullptr) {
        cerr << "Failed to open GLFW window. .... \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);    

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {        
        cerr << "Failed to initialize GLEW\n";
        exit(-1);
    }    

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    return window;
}

void countFrames(int &frames, double currentFrame, double lastFrame) {
    static double timePassed = 0;
    timePassed += currentFrame - lastFrame;
    if(timePassed >= 1) {
        cout << frames << " fps\n";
        frames = 0;
        timePassed = 0;
    }
    frames++;
}

int main() {
    auto window = GetGLFWwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL terrain rendering");

    glEnable(GL_DEBUG_OUTPUT);
    GetFirstNMessages(10);

    Plane plane(WINDOW_WIDTH, WINDOW_HEIGHT, 30, 30, &camera);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;

    while(glfwWindowShouldClose(window) == 0) {     
        double time = glfwGetTime();
        deltaTime = time - prevFrameTime;
        countFrames(frames, time, prevFrameTime);
        prevFrameTime = time;

        glfwPollEvents();
        camera.Move(keys, deltaTime);
        glClearColor(0.2, 0.2, 0, 2);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        plane.Draw();
        GetFirstNMessages(10);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
