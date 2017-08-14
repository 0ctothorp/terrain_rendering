#pragma once

#include <string>
#include <vector>
#include <fstream>


class HMParser {
private:
    enum PointTo_ {
        PointTo_Right,
        PointTo_Left,
        PointTo_Bottom,
        PointTo_Top,
        PointTo_BottomRight,
        PointTo_BottomLeft,
        PointTo_TopRight,
        PointTo_TopLeft
    };

    int totalWidth;
    std::vector<short> data;
    std::vector<short> normals;
    std::vector<std::ifstream> heightmapFiles;
    const std::vector<std::string> heightmapPaths;

    void TryToOpenAFile(const std::string& path, std::ifstream& stream);
    void ParseHeightmaps();
    void TryToReadAFile(int heightmapsInRow, int row, int positionInRow);
    void CloseFiles();
    void CalculateNormals();
    glm::vec3 GetTriangleNormal(glm::vec3 point, glm::vec3 p1, glm::vec3 p2);
    glm::vec3 GetPointTo(glm::vec3 point, PointTo_ pointTo);
    short swapBytes(short s);
    glm::vec3 GetTriangleNormalTo(glm::vec3 point, PointTo_ dir);

public:
    const int width = 1201;
    
    HMParser(const std::vector<std::string>& heightmaps);
    std::vector<short>* GetDataPtr();
    std::vector<short>* GetNormalsPtr();
    int GetTotalWidth();
};