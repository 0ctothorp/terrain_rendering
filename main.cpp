#include <iostream>
#include <ctime>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cpr/cpr.h>

#include "libs/imgui/imgui.h"
#include "libs/imgui/imgui_impl_glfw_gl3.h"
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_GIF
#include "libs/stb/stb_image.h"

#include "mainCamera.hpp"
#include "topCamera.hpp"
#include "glDebug.hpp"
#include "mouse.hpp"
// #include "lodPlane.hpp"
#include "window.hpp"
#include "topViewFb.hpp"
#include "topViewScreenQuad.hpp"
#include "cmdLineArgs.hpp"
#include "tileGeometry.hpp"
#include "normalMapQuad.hpp"
#include "framebufferSizeChangedEvent.hpp"


bool keys[GLFW_KEY_LAST]{false};
bool cursor = false;
bool wireframeMode = false;
bool meshMovementLocked = false;
bool drawTerrainNormals = false;
bool drawTopView = false;
bool terrainVertexSnapping = true;
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

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    GL_CHECK(glViewport(0, 0, width, height));
    FramebufferSizeChangedEvent::Fire();
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_CULL_FACE);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    return window;
}

int random(int min, int max) {
    float rand = std::rand() / (float)RAND_MAX;
    return min + (max - min) * rand;
}

class Img {
private:
    unsigned char* data;
    int sizeX, sizeY, channels;
public:
    Img(std::string filePath, int request_components=0) {
        data = stbi_load(filePath.c_str(), &sizeX, &sizeY, &channels, request_components);
    }

    ~Img()                   { stbi_image_free(data); }
    int GetSizeX()           { return sizeX; }
    int GetSizeY()           { return sizeY; }
    int GetChannels()        { return channels; }
    unsigned char* GetData() { return data; }
};

struct HeightmapDownloadInfo {
    int lat = 50, lon = 15, size = 4;
};

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

    auto window = GetGLFWwindow("OpenGL terrain rendering");
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    ImGui_ImplGlfwGL3_Init(window, false);

    Img availableDataImg("img/Continent_def.gif", 3);
    
    class Quad : public Drawable, public FramebufferSizeChangeListener {     
    private:
        GLuint texId;
        GLfloat vertices[30] {
            // vertices  // tex coords
            -1,  1, 0,   0, 0,
            -1, -1, 0,   0, 1,
             1,  1, 0,   1, 0,
             1,  1, 0,   1, 0,
            -1, -1, 0,   0, 1,
             1, -1, 0,   1, 1            
        };
        float imgRatio;
        int imgSizeX, imgSizeY;

        void PreserveImgRatio() {
            const float screenRatio = (float)Window::width / (float)Window::height;
            float modX = 0, modY = 0;
            if(screenRatio > imgRatio) {
                float yFactor = (float)Window::height / (float)imgSizeY;
                int shrinkedImgX = imgSizeX * yFactor;
                int xDiff = Window::width - shrinkedImgX;
                modX = xDiff / (float)Window::width;
            } else if(imgRatio > screenRatio) {
                float xFactor = (float)Window::width / (float)imgSizeX;
                int shrinkedImgY = imgSizeY * xFactor;
                int yDiff = Window::height - shrinkedImgY;
                modY = yDiff / (float)Window::height;
            }
            vertices[0] = vertices[5] = vertices[20] = -1 + modX;
            vertices[10] = vertices[15] = vertices[25] = 1 - modX;
            vertices[1] = vertices[11] = vertices[16] = 1 - modY;
            vertices[6] = vertices[21] = vertices[26] = -1 + modY;
        }

        void ModifyBuffer() {
            PreserveImgRatio();
            BindVbo();
            GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices));
            UnbindVbo();
        }

    public:
        Quad(Img& img) 
        : Drawable("shaders/framebufferVertexShader.glsl", "shaders/framebufferFragmentShader.glsl") {
            imgSizeX = img.GetSizeX();
            imgSizeY = img.GetSizeY(); 
            imgRatio = imgSizeX / imgSizeY;
            PreserveImgRatio();
            BindVbo();
            GL_CHECK(glBufferData(
                GL_ARRAY_BUFFER,
                sizeof(vertices),
                &vertices,
                GL_DYNAMIC_DRAW
            ));
            BindVao();
            GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0)); 
            GL_CHECK(glEnableVertexAttribArray(0));
            GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 
                                           (GLvoid*)(3 * sizeof(GLfloat)))); 
            GL_CHECK(glEnableVertexAttribArray(1));
            UnbindVao();
            UnbindVbo();

            GL_CHECK(glGenTextures(1, &texId));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, texId));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, img.GetSizeX(), img.GetSizeY(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.GetData()));

            shader.Uniform1i("screenTexture", 4);

            FramebufferSizeChangedEvent::Register(this);
        }

        ~Quad() {
            glDeleteTextures(1, &texId);
        }

        void Draw() {
            shader.Use();
            BindVao();
            GL_CHECK(glActiveTexture(GL_TEXTURE4));
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, texId));
            GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
            UnbindVao();
        }

        void OnFramebufferSizeChange() {
            ModifyBuffer();
        }
    };

    Quad availableDataTexturedQuad(availableDataImg);

    std::vector<std::string> heightmaps = GetHeightmapsPathsFromCmdLine(argv[1], heightmapsInRow);    
    // LODPlane lodPlane(heightmaps, planeWidth);
    TopCamera topCam(1500.0f);
    TopViewFb topViewFb(Window::width, Window::height);
    TopViewScreenQuad topViewScreenQuad(&topViewFb);

    // NormalMapQuad nmq(lodPlane.GetNormalMapTex());

    // lodPlane.shader.Uniform1i("vertexSnapping", (int)terrainVertexSnapping);
    // lodPlane.normalsShader.Uniform1i("vertexSnapping", (int)terrainVertexSnapping);
    // lodPlane.shader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
    // lodPlane.normalsShader.UniformMatrix4fv("projMat", mainCamera.projectionMat);

    std::srand(std::time(0));
    float lightPos[3]{(float)random(500, 2000), (float)1000, (float)random(500, 2000)};
    
    bool prevMeshMovementLocked = meshMovementLocked;
    bool prevTerrainVertexSnapping = terrainVertexSnapping;
    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;
    int fps = 0;
    double timePassed = 0;
    bool show_info_window = true;
    bool show_settings_window = true;
    int infoWindowWidth = 0;
    bool debugColors = false;
    bool prevDebugColors = debugColors;
    bool prevDrawTerrainNormals = drawTerrainNormals;

    enum Light_ {Light_PrecalcNormals, Light_LivecalcNormals, Light_None};
    int lightingType = Light_None;
    int prevLightingType = lightingType;

    HeightmapDownloadInfo heightmapDownloadInfo;

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

        // {
        //     ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        //     ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));
        //     ImGuiWindowFlags infoWindowFlags = 0;
        //     infoWindowFlags |= ImGuiWindowFlags_NoResize;
        //     infoWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        //     ImGui::SetNextWindowPos(ImVec2(Window::width - infoWindowWidth - 10, 10), ImGuiSetCond_Always);
        //     ImGui::Begin("Info", &show_info_window, infoWindowFlags);
        //     infoWindowWidth = ImGui::GetWindowWidth();
        //     ImGui::Text("FPS: %d", fps);
        //     ImGui::Separator();
        //     ImGui::Text("ESC to unlock mouse cursor.\nESC again to lock cursor.");      
        //     ImGui::End();
        // }
        
        // int settingsWindowWidth = 325;
        // int settingsWindowHeight = 350;
        // {
        //     ImGui::SetNextWindowPos(ImVec2(Window::width - settingsWindowWidth - 10, Window::height - settingsWindowHeight - 10), ImGuiSetCond_FirstUseEver);
        //     ImGui::SetNextWindowSize(ImVec2(settingsWindowWidth, settingsWindowHeight), ImGuiSetCond_FirstUseEver);
        //     ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        //     ImGuiWindowFlags settingsWindowFlags = 0;
        //     ImGui::Begin("Settings", &show_settings_window, settingsWindowFlags);
        //     ImGui::TextWrapped("Press ESC to unlock cursor, then check desired checkboxes and then press ESC again "
        //                        "to lock cursor and get back camera control.");

        //     ImGui::Separator();

        //     ImGui::Checkbox("Wireframe (R)", &wireframeMode);
        //     ImGui::Checkbox("Lock terrain mesh in place (L)", &meshMovementLocked);   
        //     ImGui::Checkbox("Draw terrain normals (N)", &drawTerrainNormals);
        //     ImGui::Checkbox("Draw top view (T)", &drawTopView);
        //     ImGui::Checkbox("Terrain vertex snapping (V)", &terrainVertexSnapping);
        //     ImGui::Checkbox("Color mesh levels", &debugColors);

        //     ImGui::Separator();

        //     ImGui::Text("Lighting");
        //     ImGui::DragFloat3("Light position", lightPos);     
        //     ImGui::RadioButton("Precalculated normals", &lightingType, Light_PrecalcNormals);
        //     ImGui::RadioButton("Live calculated normals", &lightingType, Light_LivecalcNormals);
        //     ImGui::RadioButton("No normals (no light)", &lightingType, Light_None);

        //     ImGui::Separator();

        //     ImGui::DragFloat("Movement speed", &mainCamera.movementSpeed);

        //     ImGui::End();
        // }

        if(debugColors != prevDebugColors) {
            // lodPlane.shader.Uniform1i("debug", (int)debugColors);
        }
        prevDebugColors = debugColors;

        if(lightingType == Light_PrecalcNormals && lightingType != prevLightingType) {
            terrainVertexSnapping = true;
        } else if(lightingType == Light_LivecalcNormals && lightingType != prevLightingType) {
            terrainVertexSnapping = true;
        } else if(lightingType != prevLightingType) {
            terrainVertexSnapping = true;
        } 
        prevLightingType = lightingType;

        // lodPlane.shader.Uniform1i("lightType", lightingType);
        // lodPlane.normalsShader.Uniform1i("lightType", lightingType);

        if(terrainVertexSnapping != prevTerrainVertexSnapping) {
            // lodPlane.shader.Uniform1i("vertexSnapping", (int)terrainVertexSnapping);
            // lodPlane.normalsShader.Uniform1i("vertexSnapping", (int)terrainVertexSnapping);
        }
        prevTerrainVertexSnapping = terrainVertexSnapping;

        mainCamera.Move(keys, deltaTime);
        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CHECK(glEnable(GL_DEPTH_TEST));

        if(prevMeshMovementLocked != meshMovementLocked)
            // lodPlane.ToggleMeshMovementLock(mainCamera);  
        prevMeshMovementLocked = meshMovementLocked;

        if(wireframeMode)
            GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));

        // lodPlane.DrawFrom(mainCamera);

        if(drawTerrainNormals != prevDrawTerrainNormals)
            // lodPlane.ToggleDebugNormals();
        prevDrawTerrainNormals = drawTerrainNormals;

        if(wireframeMode)
            GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

        if(drawTopView)
            // topViewFb.Draw(lodPlane, &topCam, &mainCamera);

        // lodPlane.shader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
        // lodPlane.normalsShader.UniformMatrix4fv("projMat", mainCamera.projectionMat);

        if(drawTopView)
            topViewScreenQuad.Draw();

        // nmq.Draw();

        availableDataTexturedQuad.Draw();

        {
            ImGui::Begin("Where do you want to go?");
            ImGui::TextWrapped(
                "The image beneath this window shows what data is available to display. "
                "Please provide latitude and longitude of the colored square that you want to explore. "
                "You can move this window around if you want to. "
            );
            ImGui::Separator();
            ImGui::DragInt("Latitude", &heightmapDownloadInfo.lat, -170, 170);
            ImGui::DragInt("Longitude", &heightmapDownloadInfo.lon, .25, -60, 60);
            ImGui::Separator();
            ImGui::TextWrapped(
                "The size of 2 means, that the application will display a bigger square of data consisting of 2 by 2 little "
                "squares extending to the right and the bottom of the one chosen above."
            );
            ImGui::InputInt("Size", &heightmapDownloadInfo.size);
            if(ImGui::Button(" OK ")) {
                // download heightmaps
            }
            ImGui::End();
        }

        ImGui::Render();
        // lodPlane.shader.Uniform3f("lightPosition", lightPos[0], lightPos[1], lightPos[2]);        
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
