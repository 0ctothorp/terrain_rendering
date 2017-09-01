#pragma once

#include <vector>


class HeightmapDownloadProgressListener {
public:
    virtual void OnHeightmapDownloadProgress(float progress) = 0;
};

class HeightmapDownloadProgressEvent {
private:
    static std::vector<HeightmapDownloadProgressListener*> listeners;

public:
    static void Fire(float progress) {
        for(auto listener: listeners)
            listener->OnHeightmapDownloadProgress(progress);
    }

    static void Register(HeightmapDownloadProgressListener *listener) {
        listeners.push_back(listener);
    }
};

std::vector<HeightmapDownloadProgressListener*> HeightmapDownloadProgressEvent::listeners{};