#pragma once

#include <string>
#include <vector>
#include <fstream>


class HMParser {
private:
    int totalWidth;
    std::vector<short> data;
    std::vector<std::ifstream> heightmapFiles;
    const std::vector<std::string> heightmapPaths;

    void TryToOpenAFile(const std::string& path, std::ifstream& stream);
    void ParseHeightmaps();
    void TryToReadAFile(int heightmapsInRow, int row, int positionInRow);
    void CloseFiles();
public:
    const int width = 1201;
    
    HMParser(const std::vector<std::string>& heightmaps);
    std::vector<short>* GetDataPtr();
};