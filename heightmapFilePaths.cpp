#include <sstream>
#include <iomanip>
#include <fstream>

#include "heightmapFilePaths.hpp"


std::vector<std::string> GetHeightmapsFilenamesBasedOn(const char* path, int heightmapsInRow) {
    std::string heightmapPath(path);
    std::vector<std::string> heightmaps;
    std::string latlon = heightmapPath.substr(heightmapPath.size() - 11, 7);
    std::string directories = heightmapPath.substr(0, heightmapPath.size() - 11);
    int lat = std::stoi(latlon.substr(1, 2));
    int lon = std::stoi(latlon.substr(4, 3));
    for(int i = 0; i < heightmapsInRow; i++) {
        int tmplat = lat - i;
        char tmplatDirection = latlon[0];
        if(tmplat < 0 && tmplatDirection == 'N') {
            tmplat = -tmplat;
            tmplatDirection = 'S';
        } else if(tmplat <= 0 && tmplatDirection == 'S') {
            tmplat = -tmplat;
            tmplatDirection = 'N';
        }
        for(int j = 0; j < heightmapsInRow; j++) {
            int tmplon = lon + j;
            char tmplonDirection = latlon[3];
            if(tmplon > 180 && tmplonDirection == 'W') {
                tmplon = 360 - tmplon;
                tmplonDirection = 'E';
            } else if(tmplon > 179 && tmplonDirection == 'E') {
                tmplon = 360 - tmplon;
                tmplonDirection = 'W';
            } else if(tmplon < 0 && tmplonDirection == 'E') {
                tmplon = -tmplon;
                tmplonDirection = 'W';
            } else if(tmplon <= 0 && tmplonDirection == 'W') {
                tmplon = -tmplon;
                tmplonDirection = 'E';
            }
            std::string filename = GetHeightmapFileName(tmplat, tmplon, tmplatDirection, tmplonDirection);
            heightmaps.push_back(directories + filename);
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

std::string GetHeightmapFileNameWithPossibleNegativeLatLon(int lat, int lon) {
    int tmplat = lat, tmplon = lon;
    char latDir, lonDir;
    if(lat < 0) {
        tmplat = -lat;
        latDir = 'S';
    } else {
        latDir = 'N';
    }
    if(lon < 0) {
        tmplon = -tmplon;
        lonDir = 'W';
    } else {
        lonDir = 'E';
    }
    return GetHeightmapFileName(tmplat, tmplon, latDir, lonDir);
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