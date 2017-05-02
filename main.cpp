#include <iostream>
#include <vector>
#include <memory>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "camera.hpp"
#include "glDebug.hpp"
#include "mouse.hpp"
#include "HMParser.hpp"
#include "lodPlane.hpp"

using namespace std;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

Camera camera(glm::vec3(0, 5, 0));
Mouse mouse(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, &camera);

const int MAX_KEY_CODE = 348;
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
    mouse.MoveCallback(xpos, ypos);
}

void MouseScrollCallback(GLFWwindow* window, double, double yoffset) {
    mouse.ScrollCallback(yoffset);
}

GLFWwindow* GetGLFWwindow(int &w, int &h, const char *name){
    GLFWwindow* window;
    if(!glfwInit()) {
        cerr << "Failed to initialize GLFW\n";
        exit(-1);
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

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

    glewExperimental = GL_TRUE; // Needed for core profile
    GLenum err;
    if ((err = glewInit()) != GLEW_OK) {        
        cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << "\n";
        exit(-1);
    }    

    // GetFirstNMessages(10);

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);

    GetErrors();

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

int main(int argc, char **argv) {
    HMParser hmParser("N50E016.hgt");

    int planeW = 0, planeH = 0;
    if(argc > 1) {
        planeW = atoi(argv[1]);
        planeH = atoi(argv[2]);
    }

    auto window = GetGLFWwindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL terrain rendering");

    // Musi być zainicjalizowane tutaj, bo w global scope nie ma zainicjalizowanego GLEW'a.
    TileMaterial::shader = new Shader{TileMaterial::vertexShaderPath, 
                                      TileMaterial::fragmentShaderPath};
    TileMesh::SetTileGeom();

    LODPlane lodPlane(WINDOW_WIDTH, WINDOW_HEIGHT, &camera);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glEnable(GL_CULL_FACE);

    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;

    GetErrors();

    while(glfwWindowShouldClose(window) == 0) {     
        double time = glfwGetTime();
        deltaTime = time - prevFrameTime;
        countFrames(frames, time, prevFrameTime);
        prevFrameTime = time;

        glfwPollEvents();
        camera.Move(keys, deltaTime);
        glClearColor(0.0, 0.0, 0.0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lodPlane.Draw();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
}
