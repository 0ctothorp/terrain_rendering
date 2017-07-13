#include <iostream>
#include <vector>

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
#include "HMParser.hpp"
#include "lodPlane.hpp"
#include "window.hpp"
#include "framebuffer.hpp"

using namespace std;


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
    if(!cursor) Mouse::GetInstance()->MoveCallback(xpos, ypos);
}

void MouseScrollCallback(GLFWwindow* window, double, double yoffset) {
    Mouse::GetInstance()->ScrollCallback(yoffset);
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

    GL_CHECK(glViewport(0, 0, Window::width, Window::height));
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

    TileMesh::SetTileGeom();
    
    LODPlane lodPlane;
    HMParser hmParser("heightmaps/N50E016.hgt");
    lodPlane.SetHeightmap(hmParser.GetDataPtr());

    TopCamera topCam(1000.0f);
    // Rozdzielczość tekstury tego frambuffera musi być taka sama, jak rozdzielczość okna,
    // żeby wszytsko prawidłowo wyglądało.
    Framebuffer topViewFb(1280, 720);

    const float screenRatio = (float)Window::width / (float)Window::height;
    const float topViewXsize = 0.6f;
    const float topViewYsize = ((topViewXsize * topViewFb.GetResHeight()) / 
                               (float)topViewFb.GetResWidth()) * screenRatio;


    GLuint topViewVao, topViewVbo;
    GLfloat topViewQuad[] = {
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1.0f, 0.0f, 1.0f, 1.0f,
        -1 + topViewXsize, 1 - topViewYsize, 0.0f, 1.0f, 0.0f,
        -1.0f, 1 - topViewYsize, 0.0f, 0.0f, 0.0f
    };
    GL_CHECK(glGenVertexArrays(1, &topViewVao));
    GL_CHECK(glBindVertexArray(topViewVao));
    GL_CHECK(glGenBuffers(1, &topViewVbo));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, topViewVbo));       
    GL_CHECK(glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(topViewQuad) * sizeof(GLfloat),
        &topViewQuad,
        GL_STATIC_DRAW
    ));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                   (GLvoid*)(3 * sizeof(GLfloat)))); 
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    Shader topViewShader("shaders/framebufferVertexShader.glsl", 
                         "shaders/framebufferFragmentShader.glsl");
    topViewShader.Use();
    GL_CHECK(glUniform1i(topViewShader.GetUniform("screenTexture"), 1));
    glm::mat4 topViewProjMat = glm::perspective(glm::radians(60.0f), 
                                                (float)topViewFb.GetResWidth() / 
                                                (float)topViewFb.GetResHeight(), 
                                                0.1f, 3000.0f);


    int w1, h1;
    glfwGetWindowSize(window, &w1, &h1);
    int w2, h2;
    glfwGetFramebufferSize(window, &w2, &h2);
    assert(w1 == w2);
    assert(h1 == h2);

    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;
    bool show_test_window = true;
    while(glfwWindowShouldClose(window) == 0) {    
        lodPlane.shader.Use();
        glActiveTexture(GL_TEXTURE0);

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

        MainCamera::GetInstance()->Move(keys, deltaTime);
        glm::vec3 mainCamPos = MainCamera::GetInstance()->GetPosition();
        TileMesh::SetGlobalOffset(mainCamPos.x, mainCamPos.z);
        lodPlane.shader.Use();
        GL_CHECK(glUniform2f(lodPlane.shader.GetUniform("globalOffset"), mainCamPos.x, 
                             mainCamPos.z));
        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        lodPlane.DrawFrom(*MainCamera::GetInstance());

        GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

        topViewFb.Bind();
        GL_CHECK(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)); // we're not using the stencil buffer now
        GL_CHECK(glEnable(GL_DEPTH_TEST));
        GL_CHECK(glActiveTexture(GL_TEXTURE0));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, lodPlane.GetHeightmapTexture()));
        GL_CHECK(glUniformMatrix4fv(lodPlane.shader.GetUniform("projMat"), 
                                    1, GL_FALSE, glm::value_ptr(topViewProjMat)));
        topCam.SetPosition(glm::vec3(mainCamPos.x, topCam.GetPosition().y, mainCamPos.z));
        lodPlane.DrawFrom(*MainCamera::GetInstance(), &topCam);
        topViewFb.Unbind();

        GL_CHECK(glUniformMatrix4fv(lodPlane.shader.GetUniform("projMat"), 
                                    1, GL_FALSE, glm::value_ptr(lodPlane.projectionMatrix)));

        topViewShader.Use();
        GL_CHECK(glDisable(GL_DEPTH_TEST));
        GL_CHECK(glBindVertexArray(topViewVao));
        GL_CHECK(glActiveTexture(GL_TEXTURE1));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, topViewFb.GetColorTexture()));
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
