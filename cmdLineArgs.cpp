#include <sstream>
#include <iomanip>

#include "cmdLineArgs.hpp"


std::vector<std::string> GetHeightmapsPathsFromCmdLine(const char* path, int heightmapsInRow) {
    std::string heightmapPath(path);
    std::vector<std::string> heightmaps;
    std::string latlon = heightmapPath.substr(heightmapPath.size() - 11, 7);
    std::string directories = heightmapPath.substr(0, heightmapPath.size() - 11);
    int lat = std::stoi(latlon.substr(1, 2));
    int lon = std::stoi(latlon.substr(4, 3));
    for(int i = 0; i < heightmapsInRow; i++) {
        int tmplat = lat - i;
        for(int j = 0; j < heightmapsInRow; j++) {
            int tmplon = lon + j;
            std::stringstream sstream;
            sstream << directories 
                    << latlon[0]
                    << std::setfill('0') << std::setw(2) << tmplat
                    << latlon[3]
                    << std::setw(3) << tmplon
                    << ".hgt";
            heightmaps.push_back(sstream.str());
        }
    }
    return heightmaps;
}