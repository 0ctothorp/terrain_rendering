#pragma once

#include <mutex>


struct HeightmapDownloadInfo {
    int lat = 50, lon = 15, size = 4;
    int downloadedCount;
    std::mutex downloadedCountMutex;

    int GetTotalSize() { return size * size; }
    
    float GetDownloadedPercent() {
        return (float)downloadedCount / GetTotalSize();
    }
};