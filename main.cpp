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
#include "cmdLineArgs.hpp"
#include "tileGeometry.hpp"

bool keys[GLFW_KEY_LAST]{false};
bool cursor = false;
bool wireframeMode = false;
bool meshMovementLocked = false;
bool drawTerrainNormals = false;
bool drawTopView = false;
bool terrainVertexSnapping = false;
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

    if(action == GLFW_PRESS) {
        keys[key] = true;
        switch(key) {
            case GLFW_KEY_R:
                wireframeMode = !wireframeMode;
                break;
            case GLFW_KEY_L:
                meshMovementLocked = !meshMovementLocked;
                break;
            case GLFW_KEY_N:
                drawTerrainNormals = !drawTerrainNormals;
                break;
            case GLFW_KEY_T:
                drawTopView = !drawTopView;
                break;
            case GLFW_KEY_V:
                terrainVertexSnapping = !terrainVertexSnapping;
            default:
                break;
        }
    } else if(action == GLFW_RELEASE) {
        keys[key] = false;
    }
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
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

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
    if(argc >= 4)
        planeWidth = std::stoi(argv[3]);
    if(argc >= 6) {
        Window::width = std::stoi(argv[4]);
        Window::height = std::stoi(argv[5]);
    }
    if(argc >= 7)
        TileGeometry::tileSize = std::stoi(argv[6]);

    std::vector<std::string> heightmaps = GetHeightmapsPathsFromCmdLine(argv[1], heightmapsInRow);    

    auto window = GetGLFWwindow("OpenGL terrain rendering");
    ImGui_ImplGlfwGL3_Init(window, false);
    
    LODPlane lodPlane(heightmaps, planeWidth, "shaders/planeVertexShader.glsl",
                      "shaders/planeFragmentShader.glsl", "");
    LODPlane lodPlaneNormalsDebug(heightmaps, planeWidth, "shaders/planeVertexShader.glsl",
                                  "shaders/normalsFragment.glsl", "shaders/normalsGeom.glsl");
    lodPlaneNormalsDebug.points = true;
    TopCamera topCam(1500.0f);
    TopViewFb topViewFb(Window::width, Window::height);
    TopViewScreenQuad topViewScreenQuad(&topViewFb);

    float lightPos[3]{0, 1000, 0};

    bool prevMeshMovementLocked = meshMovementLocked;
    bool prevTerrainVertexSnapping = terrainVertexSnapping;
    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;
    int fps = 0;
    bool show_test_window = false;
    double timePassed = 0;
    bool show_info_window = true;
    bool show_settings_window = true;
    int infoWindowWidth = 0;
    while(glfwWindowShouldClose(window) == 0) {    
        double time = glfwGetTime();
        deltaTime = time - prevFrameTime;
        timePassed += deltaTime;
        if(timePassed >= 1.0) {
            fps = frames;
            frames = 0;
            timePassed = 0;
        }
        frames++;
        prevFrameTime = time;

        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));
        ImGuiWindowFlags infoWindowFlags = 0;
        infoWindowFlags |= ImGuiWindowFlags_NoResize;
        infoWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::SetNextWindowPos(ImVec2(Window::width - infoWindowWidth - 10, 10), ImGuiSetCond_Always);
        ImGui::Begin("Info", &show_info_window, infoWindowFlags);
        infoWindowWidth = ImGui::GetWindowWidth();
        ImGui::Text("FPS: %d", fps);
        ImGui::Separator();
        ImGui::Text("ESC to unlock mouse cursor.\nESC again to lock cursor.");
        ImGui::End();
        
        int settingsWindowWidth = 250;
        ImGui::SetNextWindowPos(ImVec2(Window::width - settingsWindowWidth - 10, 95), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(settingsWindowWidth, 200), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        ImGuiWindowFlags settingsWindowFlags = 0;
        ImGui::Begin("Settings", &show_settings_window, settingsWindowFlags);
        ImGui::TextWrapped("Press ESC to unlock cursor, then check desired checkboxes and then press ESC again to lock cursor and get back camera control.");
        ImGui::Separator();
        ImGui::Checkbox("Wireframe (R)", &wireframeMode);
        ImGui::Checkbox("Lock terrain mesh in place (L)", &meshMovementLocked);   
        ImGui::Checkbox("Draw terrain normals (N)", &drawTerrainNormals);
        ImGui::Checkbox("Draw top view (T)", &drawTopView);
        ImGui::Checkbox("Terrain vertex snapping (V)", &terrainVertexSnapping);
        ImGui::Separator();
        ImGui::DragFloat3("Light position", lightPos);     
        ImGui::End();

        if(terrainVertexSnapping != prevTerrainVertexSnapping) {
            lodPlane.shader.Uniform1i("vertexSnapping", (int)terrainVertexSnapping);
        }
        prevTerrainVertexSnapping = terrainVertexSnapping;

        lodPlane.shader.Uniform3f("lightPosition", lightPos[0], lightPos[1], lightPos[2]);

        mainCamera.Move(keys, deltaTime);
        glm::vec3 mainCamPos = mainCamera.GetPosition();
        TileMesh::SetGlobalOffset(mainCamPos.x, mainCamPos.z);
        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CHECK(glEnable(GL_DEPTH_TEST));

        if(prevMeshMovementLocked != meshMovementLocked) {
            lodPlane.ToggleMeshMovementLock(mainCamera);  
            lodPlaneNormalsDebug.ToggleMeshMovementLock(mainCamera);
        }  
        prevMeshMovementLocked = meshMovementLocked;

        if(wireframeMode)
            GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

        lodPlane.DrawFrom(mainCamera);

        if(drawTerrainNormals)
            lodPlaneNormalsDebug.DrawFrom(mainCamera);
        if(wireframeMode)
            GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

        if(drawTopView)
            topViewFb.Draw(lodPlane, &topCam, &mainCamera);

        lodPlane.shader.UniformMatrix4fv("projMat", lodPlane.projectionMatrix);
        lodPlaneNormalsDebug.shader.UniformMatrix4fv("projMat", 
            lodPlaneNormalsDebug.projectionMatrix);

        if(drawTopView)
            topViewScreenQuad.Draw();

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
