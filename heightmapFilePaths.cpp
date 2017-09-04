#include <sstream>
#include <iomanip>
#include <fstream>
//tmp
#include <iostream>

#include "heightmapFilePaths.hpp"


std::vector<std::string> GetHeightmapsFilenamesBasedOn(int lat, int lon, int heightmapsInRow) {
    std::vector<std::string> heightmaps;
    for(int i = 0; i < heightmapsInRow; i++) {
        int tmplat = lat - i;
        char tmplatDirection = 'N';
        if(tmplat < 0) {
            tmplatDirection = 'S';
            tmplat *= -1;
        }
        for(int j = 0; j < heightmapsInRow; j++) {
            char tmplonDirection = 'E';
            int tmplon = lon + j;
            if(tmplon < 0) {
                tmplonDirection = 'W';
                tmplon *= -1;
            }
            std::string filename = GetHeightmapFileName(tmplat, tmplon, tmplatDirection, tmplonDirection);
            heightmaps.push_back(filename);
        }
    }
    return heightmaps;
}

std::string GetHeightmapFileName(int lat, int lon, char latDirection, char lonDirection) {
    std::stringstream sstream;
    sstream << latDirection
            << std::setfill('0') << std::setw(2) << lat
            << lonDirection
            << std::setw(3) << lon
            << ".hgt";
    return sstream.str();
}

std::vector<std::string> GetFilesToDownloadFrom(const std::vector<std::string>& filenames) {
    std::vector<std::string> toDownload;
    for(auto file: filenames) {
        std::ifstream fs("heightmaps/" + file);
        if(!fs.is_open())
            toDownload.push_back(file);
    }
    return toDownload;
}