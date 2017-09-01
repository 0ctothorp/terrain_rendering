#pragma once

#include "libs/imgui/imgui.h"

#include "heightmapDownloadProgressEvent.hpp"


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

class UIWindowHeightmapDownloadProgress : public UIWindow, public HeightmapDownloadProgressListener {
private:
    float progress = 0;

    virtual void Render() {
        ImGui::Begin("Downloading heightmaps");
        ImGui::ProgressBar(progress);
        ImGui::End();
    };

public:
    void OnHeightmapDownloadProgress(float _progress) {
        progress = _progress;
    }
};

class UIWindowHeightmapUnzipProgress : public UIWindow {
private:
    float progress = 0;

    virtual void Render() {
        ImGui::Begin("Unzipping heightmaps");
        ImGui::ProgressBar(progress);
        ImGui::End();
    };

public:
    void OnHeightmapUnzipProgress(float _progress) {
        progress = _progress;
    }
};