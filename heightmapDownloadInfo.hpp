#pragma once

#include <mutex>


struct HeightmapDownloadInfo {
    int lat = 50, lon = 15, size = 4;
    int downloadedCount = 0;
    int unzippedCount = 0;
    std::mutex downloadedCountMutex;

    int GetTotalSize() { return size * size; }
    
    float GetDownloadedPercent() {
        return (float)downloadedCount / GetTotalSize();
    }
};