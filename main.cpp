#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw_gl3.h"

#include "mainCamera.hpp"
#include "topCamera.hpp"
#include "glDebug.hpp"
#include "mouse.hpp"
#include "lodPlane.hpp"
#include "window.hpp"
#include "topViewFb.hpp"
#include "topViewScreenQuad.hpp"
#include "utils.hpp"
#include "cmdLineArgs.hpp"


const int MAX_KEY_CODE = 348;
bool keys[MAX_KEY_CODE]{false};
bool cursor = false;
MainCamera mainCamera;
Mouse mouse((float)Window::width / 2.0f, (float)Window::height / 2.0f, &mainCamera);

void KeyCallback(GLFWwindow* window, int key, int, int action, int) {
    if(key == -1) {
        std::cerr << "Pressed unknown key\n";
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
    std::cerr << "[GLFW ERROR " << error << "] " << description << '\n';
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
        std::cerr << "Failed to initialize GLFW\n";
        exit(-1);
    }

    glfwSetErrorCallback(GlfwErrorCallback);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    if (Window::width == 0 || Window::height == 0) {
        // if 0 then Window.width FULL SCREEN :
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        window = glfwCreateWindow(mode->width, mode->height, name, monitor, nullptr);
        Window::width = mode->width; 
        Window::height = mode->height;
    } else {
        window = glfwCreateWindow(Window::width, Window::height, name, nullptr, nullptr);
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(window == nullptr) {
        std::cerr << "Failed to open GLFW window. .... \n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window); 

    glewExperimental = GL_TRUE;
    GLenum err;
    if((err = glewInit()) != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << "\n";
        exit(-1);
    }   

    GL_CHECK();
    #ifdef DEBUG
    std::cerr << "Ignore preceding error.\n" << endl;
    #endif

    GL_CHECK(glViewport(0, 0, Window::width, Window::height));
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);

    return window;
}

int main(int argc, char **argv) {
    int heightmapsInRow = 1;
    int planeWidth = 1024;

    if(argc < 2) {
        std::cerr << "Wrong number of command line arguments provided.\n";
        return 1;
    }
    if(argc >= 3)
        heightmapsInRow = std::stoi(argv[2]);
    if(argc == 4)
        planeWidth = std::stoi(argv[3]);

    std::vector<std::string> heightmaps = GetHeightmapsPathsFromCmdLine(argv[1], heightmapsInRow);    

    auto window = GetGLFWwindow("OpenGL terrain rendering");
    ImGui_ImplGlfwGL3_Init(window, false);
    
    LODPlane lodPlane(heightmaps, planeWidth);
    TopCamera topCam(1500.0f);
    TopViewFb topViewFb(1280, 720);
    TopViewScreenQuad topViewScreenQuad(&topViewFb);

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

        mainCamera.Move(keys, deltaTime);
        glm::vec3 mainCamPos = mainCamera.GetPosition();
        TileMesh::SetGlobalOffset(mainCamPos.x, mainCamPos.z);
        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        lodPlane.DrawFrom(mainCamera);

        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

        topViewFb.Bind();
        topViewFb.Draw(lodPlane, &topCam, &mainCamera);
        topViewFb.Unbind();

        GL_CHECK(glUniformMatrix4fv(lodPlane.shader.GetUniform("projMat"), 
                                    1, GL_FALSE, glm::value_ptr(lodPlane.projectionMatrix)));

        topViewScreenQuad.Draw();

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
