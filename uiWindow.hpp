#pragma once

#include "libs/imgui/imgui.h"

#include "events.hpp"
#include "heightmapDownloadInfo.hpp"
#include "terrainRenderingSettings.hpp"
#include "terrainSettings.hpp"


class UIWindow {
private:
    bool isActive = false;

    virtual void Render() = 0;
public:
    void Draw() {
        if(isActive) Render();
    }; 

    void ToggleVisibility() { isActive = !isActive; }
};


class UIWindowHeightmapDownloadProgress : public UIWindow {
private:
    float progress = 0;

    void Render() override {
        ImGui::SetNextWindowSize(ImVec2((float)Window::width / 2.0f, 75.0f), ImGuiSetCond_Always);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGuiWindowFlags infoWindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Downloading heightmaps", nullptr, infoWindowFlags);
        ImGui::ProgressBar(progress);
        ImGui::End();
    };

public:
    UIWindowHeightmapDownloadProgress() {
        SingletonEvent<HeightmapsDownloadProgressEvent, float>::Instance()->Register([this](float progress){
            this->progress = progress;
        });        
        SingletonEvent<HeightmapsUnzippedEvent>::Instance()->Register([this](){
            this->ToggleVisibility();
        });
        SingletonEvent<HgtmapDownloadErrorEvent, std::string>::Instance()->Register([this](auto s){
            this->ToggleVisibility();
            progress = 0;
        });
    }
};


class UIWindowHeightmapsReading : public UIWindow {
private:
    void Render() override {
        ImGui::SetNextWindowSize(ImVec2((float)Window::width / 2.0f, 50.0f));        
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGuiWindowFlags infoWindowFlags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Please wait...", nullptr, infoWindowFlags);
        ImGui::TextWrapped("Loading heightmap files and calculating normals. Please wait...");
        ImGui::End();
    }
};


class UIWindowInfo : public UIWindow {
private:
    float width = 0;
    bool show;

    void Render() override {
        ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));
        ImGuiWindowFlags infoWindowFlags = 0;
        infoWindowFlags |= ImGuiWindowFlags_NoResize;
        infoWindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
        ImGui::SetNextWindowPos(ImVec2(Window::width - width - 10, 10), ImGuiSetCond_Always);
        ImGui::Begin("Info", &show, infoWindowFlags);
        width = ImGui::GetWindowWidth();
        ImGui::Text("FPS: %d", fps);
        ImGui::Separator();
        ImGui::Text("ESC to lock mouse cursor and get\ncontrol of camera.\nESC again to unlock cursor and get\nit back.");      
        ImGui::End();
    }

public:
    int fps = 0;
};


class UIWindowHeightmapsInfoInput : public UIWindow {
private:
    HeightmapDownloadInfo* heightmapDownloadInfo;
    TerrainSettings* terrainSettings;
    std::function<void()> onSubmit;

    void Render() override {
        ImGui::SetNextWindowSize(ImVec2(450, 350), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowPosCenter(ImGuiSetCond_Appearing);
        ImGuiWindowFlags infoWindowFlags = ImGuiWindowFlags_NoSavedSettings;
        ImGui::Begin("Where do you want to go?", nullptr, infoWindowFlags);
        ImGui::TextWrapped(
            "The image beneath this window shows what data is available to display. "
            "Please provide latitude and longitude of the colored square that you want to explore. "
            "You can move this window around if you want to. "
        );
        ImGui::Separator();
        ImGui::DragInt("Latitude", &heightmapDownloadInfo->lat, .2, -60, 60);
        ImGui::DragInt("Longitude", &heightmapDownloadInfo->lon, .2, -180, 180);
        ImGui::Separator();
        ImGui::TextWrapped(
            "The size of 2 means, that the application will display a bigger square of data consisting of 2 by 2 little "
            "squares extending to the right and the bottom of the one chosen above."
        );
        ImGui::InputInt("Size", &heightmapDownloadInfo->size);

        ImGui::Separator();
        ImGui::InputInt("LOD levels", &terrainSettings->lodLevels, 1);
        if(terrainSettings->lodLevels < 1) terrainSettings->lodLevels = 1;
        ImGui::InputInt("Tile size (2 to the power of)", &terrainSettings->tileSize, 1);
        if(terrainSettings->tileSize < 3) terrainSettings->tileSize = 3;
        ImGui::Separator();

        if(ImGui::Button(" OK "))
            onSubmit();
        ImGui::End();
    }

public:
    UIWindowHeightmapsInfoInput(std::function<void()> &&_onSubmit, HeightmapDownloadInfo* hgtDloadInfo, TerrainSettings* settings)
    : onSubmit(_onSubmit)
    , heightmapDownloadInfo(hgtDloadInfo)
    , terrainSettings(settings) {
        SingletonEvent<ReturnFromDownloadErrorEvent>::Instance()->Register([this](){
            this->ToggleVisibility();
        });
    }
};


class UIWindowRenderingSettings : public UIWindow {
private:
    int width = 325;
    int height = 350;
    bool show;
    TerrainRenderingSettings* settings;
    float *mainCamMoveSpeed;

    void Render() override {
        ImGui::SetNextWindowPos(ImVec2(Window::width - width - 10, Window::height - height - 10), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiSetCond_Appearing);
        ImGui::SetNextWindowCollapsed(false, ImGuiSetCond_FirstUseEver);
        ImGuiWindowFlags settingsWindowFlags = 0;
        ImGui::Begin("Settings", &show, settingsWindowFlags);

        ImGui::Checkbox("Wireframe (R)", &(settings->wireframeMode));
        ImGui::Checkbox("Lock terrain mesh in place (L)", &(settings->meshMovementLocked));  
        if(settings->lightingType == Light_LivecalcNormals) 
            ImGui::Checkbox("Draw terrain normals (N)", &(settings->drawTerrainNormals));
        ImGui::Checkbox("Draw top view (T)", &(settings->drawTopView));
        ImGui::Checkbox("Terrain vertex snapping (V)", &(settings->terrainVertexSnapping));
        ImGui::Checkbox("Color mesh levels", &(settings->debugColors));

        ImGui::Separator();

        ImGui::Text("Lighting");
        ImGui::DragFloat3("Light position", settings->lightPos);     
        ImGui::RadioButton("Precalculated normals", (int*)&(settings->lightingType), Light_PrecalcNormals);
        ImGui::RadioButton("Live calculated normals", (int*)&(settings->lightingType), Light_LivecalcNormals);
        ImGui::RadioButton("No normals (no light)", (int*)&(settings->lightingType), Light_None);

        ImGui::Separator();

        ImGui::DragFloat("Movement speed", mainCamMoveSpeed);
        ImGui::End();
    }

public:
    UIWindowRenderingSettings(TerrainRenderingSettings* _settings, float *_mainCamMoveSpeed) 
    : settings(_settings)
    , mainCamMoveSpeed(_mainCamMoveSpeed) {}
};


class UIDownloadError : public UIWindow {
private:
    std::string title = "Error downloadnig a file";
    std::string text = "An error occurred during downloading of file.";

    void Render() override {
        ImGui::SetNextWindowSize(ImVec2(250.0f, 150.0f));
        ImGui::SetNextWindowPosCenter();
        ImGui::Begin(title.c_str());
        ImGui::TextWrapped(text.c_str());
        if(ImGui::Button("OK")) {
            ToggleVisibility();
            SingletonEvent<ReturnFromDownloadErrorEvent>::Instance()->Fire();
        }
        ImGui::End();
    }

public:
    UIDownloadError() {
        SingletonEvent<HgtmapDownloadErrorEvent, std::string>::Instance()->Register([this](auto s){
            this->title = "Error downloadnig " + s;
            this->text = "An error occurred during downloading of " + s + " file.";
            this->ToggleVisibility();
        });
    }
};
