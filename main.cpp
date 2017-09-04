#include <iostream>
#include <ctime>
#include <fstream>
#include <sstream>
#include <future>
#include <mutex>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cpr/cpr.h>
#include <zipper/unzipper.h>

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
#include "heightmapFilePaths.hpp"
#include "tileGeometry.hpp"
#include "normalMapQuad.hpp"
#include "uiWindow.hpp"
#include "events.hpp"
#include "hmParser.hpp"
#include "heightmapDownloadInfo.hpp"
#include "terrainRenderingSettings.hpp"
#include "img.hpp"
#include "utils.hpp"
#include "terrainSettings.hpp"
#include "mapQuad.hpp"


std::string baseUrl = "https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/";
std::array<std::string, 6> continentUrls { "Eurasia/", "Africa/", "Australia/", "Islands/", "North_America/", "South_America/" };

bool keys[GLFW_KEY_LAST]{false};
bool cursor = true;

TerrainRenderingSettings settings;
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
                settings.wireframeMode = !settings.wireframeMode;
                break;
            case GLFW_KEY_L:
                settings.meshMovementLocked = !settings.meshMovementLocked;
                break;
            case GLFW_KEY_N:
                settings.drawTerrainNormals = !settings.drawTerrainNormals;
                break;
            case GLFW_KEY_T:
                settings.drawTopView = !settings.drawTopView;
                break;
            case GLFW_KEY_V:
                settings.terrainVertexSnapping = !settings.terrainVertexSnapping;
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
    SingletonEvent<FramebufferSizeChangedEvent>::Instance()->Fire();
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

/* Rozpoczyna proces asynchronicznego pobierania i rozpakowywania danych wysokości terenu w oparciu o 
   informacje zawarte w 'heightmapDownloadInfo'. 'downloadedFutures' służy do przechowania obiektów
   typu 'std::future', które odpowiedzialne są za uruchomienie wątków i przechowywanie wyników operacji. 
   Jeśli pobieranie danych nie jest potrzebne, bo znajdują się one na dysku użytkownika wywoływane jest
   w osobnym wątku zdarzenie ukończenia rozpakowywania danych. */
void DownloadHeightmaps(HeightmapDownloadInfo& heightmapDownloadInfo, std::vector<std::future<void>>& downloadedFutures, std::vector<std::string>& heightmapsFilenames) {
    int lat = heightmapDownloadInfo.lat,
        lon = heightmapDownloadInfo.lon;
    std::string filename = GetHeightmapFileNameWithPossibleNegativeLatLon(lat, lon);
    heightmapsFilenames = GetHeightmapsFilenamesBasedOn(filename.c_str(), heightmapDownloadInfo.size);
    std::vector<std::string> filesToDownload = GetFilesToDownloadFrom(heightmapsFilenames);

    if(filesToDownload.size() == 0) {
        downloadedFutures.emplace_back(std::async(std::launch::async, [](){
            SingletonEvent<HeightmapsUnzippedEvent>::Instance()->Fire();
        }));
        return;
    }

    bool errorOccurred = false;
    for(int i = 0; i < filesToDownload.size(); i++) {
        downloadedFutures.emplace_back(std::async(std::launch::async, [baseUrl, continentUrls, filename = filesToDownload[i], &heightmapDownloadInfo, i, &errorOccurred](){
            bool success = false;
            for(int cont = 0; cont < continentUrls.size(); cont++) {
                auto r = cpr::Get(cpr::Url{baseUrl + continentUrls[cont] + filename + ".zip"});
                if(r.status_code == 200) {
                    std::ofstream file("heightmaps/" + filename + ".zip");
                    file << r.text;

                    std::lock_guard<std::mutex> lock(heightmapDownloadInfo.downloadedCountMutex);
                    heightmapDownloadInfo.downloadedCount++;
                    SingletonEvent<HeightmapsDownloadProgressEvent, float>::Instance()->Fire(
                        heightmapDownloadInfo.GetDownloadedPercent()
                    );
                    SingletonEvent<HeightmapDownloadedEvent, int>::Instance()->Fire(i);
                    success = true;
                    break;
                }
            }
            std::mutex m;
            std::lock_guard<std::mutex> lock(m);
            if(!success && !errorOccurred) {
                errorOccurred = true;
                SingletonEvent<HgtmapDownloadErrorEvent, std::string>::Instance()->Fire(filename);
            }
        }));
    }
}

int main(int argc, char **argv) {
    TerrainSettings terrainSettings;

    // int heightmapsInRow = 1;
    // int planeWidth = 1024;

    // if(argc >= 2) {
    //     // nie ściągaj heightmap, tylko użyj tych z dysku
    // }
    // if(argc >= 3)
    //     heightmapsInRow = std::stoi(argv[2]);
    // if(argc >= 4)
    //     planeWidth = std::stoi(argv[3]);
    // if(argc >= 6) {
    //     Window::width = std::stoi(argv[4]);
    //     Window::height = std::stoi(argv[5]);
    // }
    // if(argc >= 7)
    //     TileGeometry::tileSize = std::stoi(argv[6]);

    auto window = GetGLFWwindow("OpenGL terrain rendering");
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
    ImGui_ImplGlfwGL3_Init(window, false);

    Img availableDataImg("img/Continent_def.gif", 3);
    MapQuad availableDataTexturedQuad(availableDataImg);

    std::unique_ptr<LODPlane> lodPlaneUptr = nullptr;
    std::unique_ptr<HMParser> hmParserUptr = nullptr;

    SingletonEvent<FramebufferSizeChangedEvent>::Instance()->Register([&mainCamera, &lodPlaneUptr](){
        if(!lodPlaneUptr) return;
        mainCamera.projectionMat = glm::perspective(
            glm::radians(mainCamera.fov), 
            (float)Window::width / (float)Window::height,
            mainCamera.near,
            mainCamera.far
        );
        lodPlaneUptr->shader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
    });

    TopCamera topCam(1500.0f);
    TopViewFb topViewFb(Window::width, Window::height);
    TopViewScreenQuad topViewScreenQuad(&topViewFb);

    // NormalMapQuad nmq(lodPlane.GetNormalMapTex());

    std::srand(std::time(0));
    settings.lightPos[0] = (float)random(-1000, 1000);
    settings.lightPos[1] = (float)1000;
    settings.lightPos[2] = (float)random(-1000, 1000);
    
    double deltaTime = 0;
    double prevFrameTime = glfwGetTime();
    int frames = 0;
    int fps = 0;
    double timePassed = 0;

    HeightmapDownloadInfo heightmapDownloadInfo;
    std::vector<std::future<void>> downloadedFutures;
    std::vector<std::string> heightmapsFilenames;
    std::future<void> futureDownload;

    UIWindowHeightmapDownloadProgress uiHgtmapDownloadProgress;
    UIWindowHeightmapsReading uiHgtmapsReading;
    UIWindowInfo uiInfo;
    UIWindowHeightmapsInfoInput uiHeightmapsInfoInput([&heightmapsFilenames, &heightmapDownloadInfo, &uiHgtmapDownloadProgress, &downloadedFutures, &uiHeightmapsInfoInput, &futureDownload](){
        futureDownload = std::async(std::launch::async, [&heightmapDownloadInfo, &downloadedFutures, &heightmapsFilenames](){
            DownloadHeightmaps(heightmapDownloadInfo, downloadedFutures, heightmapsFilenames);
        });
        uiHeightmapsInfoInput.ToggleVisibility();
        uiHgtmapDownloadProgress.ToggleVisibility();
    }, &heightmapDownloadInfo, &terrainSettings);
    uiHeightmapsInfoInput.ToggleVisibility();
    UIWindowRenderingSettings uiRenderSettings(&settings, &(mainCamera.movementSpeed));
    UIDownloadError uiDownloadError;

    // int unzippedCount = 0;
    std::mutex unzippedCountMutex;
    SingletonEvent<HeightmapDownloadedEvent, int>::Instance()->Register([&heightmapsFilenames, &unzippedCountMutex, &heightmapDownloadInfo](int index){
        try {
            zipper::Unzipper unzipper("heightmaps/" + heightmapsFilenames[index] + ".zip");
            unzipper.extract("heightmaps");
            std::lock_guard<std::mutex> lock(unzippedCountMutex);
            heightmapDownloadInfo.unzippedCount++;
        } catch(const std::runtime_error &e) {
            std::cerr << "Error unzipping " << heightmapsFilenames[index] << ": " << e.what() << std::endl;
        }
        std::lock_guard<std::mutex> lock(unzippedCountMutex);        
        if(heightmapDownloadInfo.unzippedCount == heightmapDownloadInfo.GetTotalSize())
            SingletonEvent<HeightmapsUnzippedEvent>::Instance()->Fire();
    });

    SingletonEvent<HeightmapsUnzippedEvent>::Instance()->Register([&heightmapsFilenames, &uiHgtmapsReading, &hmParserUptr, &availableDataTexturedQuad, &uiInfo, &uiRenderSettings](){      
        uiHgtmapsReading.ToggleVisibility();
        std::vector<std::string> heightmapsPaths(heightmapsFilenames);
        for(auto& name: heightmapsPaths)
            name = "heightmaps/" + name;
        hmParserUptr = std::make_unique<HMParser>(heightmapsPaths);
        uiHgtmapsReading.ToggleVisibility();
        availableDataTexturedQuad.ToggleVisibility();
        uiInfo.ToggleVisibility();
        uiRenderSettings.ToggleVisibility();
    });

    SingletonEvent<ReturnFromDownloadErrorEvent>::Instance()->Register([&downloadedFutures, &heightmapsFilenames, &heightmapDownloadInfo](){
        downloadedFutures.clear();
        heightmapsFilenames.clear();
        heightmapDownloadInfo.downloadedCount = 0;
        heightmapDownloadInfo.unzippedCount = 0;
    });

    while(glfwWindowShouldClose(window) == 0) {    
        double time = glfwGetTime();
        deltaTime = time - prevFrameTime;
        timePassed += deltaTime;
        if(timePassed >= 1.0) {
            fps = frames;
            uiInfo.fps = fps;
            frames = 0;
            timePassed = 0;
        }
        frames++;
        prevFrameTime = time;

        if(hmParserUptr) {
            mainCamera.SetPosition(glm::vec3(0, hmParserUptr->highestPoint + 5, 0));
            lodPlaneUptr = std::make_unique<LODPlane>(hmParserUptr, terrainSettings.lodLevels, terrainSettings.tileSize);
            hmParserUptr.reset(nullptr);
            lodPlaneUptr->shader.Uniform1i("vertexSnapping", (int)settings.terrainVertexSnapping);
            lodPlaneUptr->normalsShader.Uniform1i("vertexSnapping", (int)settings.terrainVertexSnapping);
            lodPlaneUptr->shader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
            lodPlaneUptr->normalsShader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
            lodPlaneUptr->shader.Uniform1i("lightType", settings.lightingType);
            lodPlaneUptr->normalsShader.Uniform1i("lightType", settings.lightingType);    
        }

        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        GL_CHECK(glClearColor(0.0, 0.0, 0.0, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        GL_CHECK(glEnable(GL_DEPTH_TEST));

        if(lodPlaneUptr) {
            mainCamera.Move(keys, deltaTime);

            if(settings.lightingType != settings.prevLightingType) {
                lodPlaneUptr->shader.Uniform1i("lightType", settings.lightingType);
                lodPlaneUptr->normalsShader.Uniform1i("lightType", settings.lightingType);    
                settings.prevLightingType = settings.lightingType;
            }
            lodPlaneUptr->shader.Uniform3f("lightPosition", settings.lightPos[0], settings.lightPos[1], settings.lightPos[2]);       
            if(settings.drawTerrainNormals != settings.prevDrawTerrainNormals) {
                lodPlaneUptr->ToggleDebugNormals();
                settings.prevDrawTerrainNormals = settings.drawTerrainNormals;
            }
            if(settings.prevMeshMovementLocked != settings.meshMovementLocked) {
                lodPlaneUptr->ToggleMeshMovementLock(mainCamera);
                settings.prevMeshMovementLocked = settings.meshMovementLocked;
            }  

            if(settings.debugColors != settings.prevDebugColors) {
                lodPlaneUptr->shader.Uniform1i("debug", (int)settings.debugColors);
                settings.prevDebugColors = settings.debugColors;
            }

            if(settings.lightingType != settings.prevLightingType) {
                settings.terrainVertexSnapping = true;
                settings.prevLightingType = settings.lightingType;
            } 

            if(settings.terrainVertexSnapping != settings.prevTerrainVertexSnapping) {
                lodPlaneUptr->shader.Uniform1i("vertexSnapping", (int)settings.terrainVertexSnapping);
                lodPlaneUptr->normalsShader.Uniform1i("vertexSnapping", (int)settings.terrainVertexSnapping);
                settings.prevTerrainVertexSnapping = settings.terrainVertexSnapping;
            }

            if(settings.wireframeMode) GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
            lodPlaneUptr->DrawFrom(mainCamera);
            if(settings.wireframeMode) GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        
            if(settings.drawTopView) {
                topViewFb.Draw(*lodPlaneUptr, &topCam, &mainCamera);
                topViewScreenQuad.Draw();
                lodPlaneUptr->shader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
                lodPlaneUptr->normalsShader.UniformMatrix4fv("projMat", mainCamera.projectionMat);
            }
        }

        // nmq.Draw();

        availableDataTexturedQuad.Draw();
        
        uiHeightmapsInfoInput.Draw();
        uiHgtmapDownloadProgress.Draw();
        uiHgtmapsReading.Draw();
        uiInfo.Draw();        
        uiRenderSettings.Draw();
        uiDownloadError.Draw();

        ImGui::Render();
        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}
