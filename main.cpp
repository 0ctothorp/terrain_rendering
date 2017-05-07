#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw_gl3.h"

#include "camera.hpp"
#include "glDebug.hpp"
#include "mouse.hpp"
#include "HMParser.hpp"
#include "lodPlane.hpp"

using namespace std;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 750;

Camera camera(glm::vec3(0, 10, 0));
Mouse mouse(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, &camera);

const int MAX_KEY_CODE = 348;
bool keys[MAX_KEY_CODE]{false};
bool cursor = false;

void KeyCallback(GLFWwindow* window, int key, int, int action, int) {
    if(key == -1) {
        cerr << "Pressed unknown key\n";
        return;
    }

    if(action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
        cursor = !cursor;   
        glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        return;
    }

    if(action == GLFW_PRESS) keys[key] = true;
    else if(action == GLFW_RELEASE) keys[key] = false;
}

void GlfwErrorCallback(int error, const char* description) {
    cerr << "[GLFW ERROR " << error << "] " << description << '\n';
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if(!cursor) mouse.MoveCallback(xpos, ypos);
}

void MouseScrollCallback(GLFWwindow* window, double, double yoffset) {
    mouse.ScrollCallback(yoffset);
}

GLFWwindow* GetGLFWwindow(const char *name){
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

    if (WINDOW_WIDTH == 0 || WINDOW_HEIGHT == 0) {
        // if 0 then WINDOWLESS FULL SCREEN :
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
        WINDOW_WIDTH = mode->width; WINDOW_HEIGHT = mode->height;
    } else {
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, name, nullptr, nullptr);
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(window == nullptr) {
        cerr << "Failed to open GLFW window. .... \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window); 

    glewExperimental = GL_TRUE;
    GLenum err;
    if((err = glewInit()) != GLEW_OK) {
        cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << "\n";
        exit(-1);
    }   

    GL_CHECK();
    cerr << "Ignore preceding error.\n" << endl;

    GL_CHECK(glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);

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
    auto window = GetGLFWwindow("OpenGL terrain rendering");
    ImGui_ImplGlfwGL3_Init(window, false);
    TileMaterial::shader = new Shader{TileMaterial::vertexShaderPath, 
                                      TileMaterial::fragmentShaderPath};
    TileMaterial::SetStaticUniforms();
    TileMesh::SetTileGeom();
    LODPlane lodPlane(WINDOW_WIDTH, WINDOW_HEIGHT, &camera);
    HMParser hmParser("heightmaps/N50E016.hgt");
    lodPlane.SetHeightmap(hmParser.GetDataPtr());

    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;
    bool show_test_window = true;
    while(glfwWindowShouldClose(window) == 0) {    
        double time = glfwGetTime();
        deltaTime = time - prevFrameTime;
        countFrames(frames, time, prevFrameTime);
        prevFrameTime = time;

        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        camera.Move(keys, deltaTime);
        GL_CHECK(glUniform2f(TileMaterial::GetUnifGlobOffset(), camera.position.x, 
                             camera.position.z));
        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        lodPlane.Draw();
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();

    delete TileMaterial::shader;
}
