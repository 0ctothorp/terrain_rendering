#pragma once

#include "libs/imgui/imgui.h"

#include "events.hpp"


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

    virtual void Render() {
        ImGui::Begin("Downloading heightmaps");
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
    }
};
