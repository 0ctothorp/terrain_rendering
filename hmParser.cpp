#include <iostream>
#include <cmath>

#include "hmParser.hpp"


HMParser::HMParser(const std::vector<std::string>& heightmaps) 
: heightmapFiles(heightmaps.size()) 
, heightmapPaths(heightmaps) {
    int i = 0;
    for(auto& hmFile: heightmapFiles) {
        hmFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        TryToOpenAFile(heightmapPaths[i++], hmFile);
    }
    ParseHeightmaps();
    CloseFiles();
    SwapBytesForAllValuesInHeightmap();
    totalWidth = (width - 1) * sqrt(heightmaps.size());
    EliminateDataVoids();    
    for(int i = 0; i < totalWidth; i++) {
        for(int j = 0; j < totalWidth; j++) {
            heights.push_back(GetHeight(data[i * totalWidth + j]));
            if(heights[heights.size() - 1] > highestPoint) highestPoint = heights[heights.size() - 1];
        }
    }
    CalculateNormals();
}

void HMParser::TryToOpenAFile(const std::string& path, std::ifstream& stream) {
    try {
        stream.open(path, std::ios_base::in | std::ios_base::binary);
    } catch(const std::ifstream::failure &e) {
        std::cerr << "[EXCEPTION: HMParser::ParseHeightmaps] " << e.what() << '\n'
                  << "Error code: " << e.code() << '\n'
                  << "There was a problem with opening a "
                  << path << " file. Please check if it exists.\n";
        exit(EXIT_FAILURE);
    }
}

void HMParser::ParseHeightmaps() {
    // - SRTM3 .hgt file has 1201 rows and 1201 samples (in each row)
    // - big-endian 16-bit signed integers - need to byte-swap
    // - byte swapping is done when texture is loaded to OpenGL (by glTexImage2D call)
    // - height data every 3 arc-seconds in both directions
    int heightmapsInRow = sqrt(heightmapPaths.size());
    for(int j = 0; j < heightmapsInRow; j++) {
        // k < width - 1, bo pierwszy i ostatni wiersz, jak i pierwsza i ostatnia kolumna
        // danych nakładają się z danymi sąsiedniej mapy wysokości. Ja łączę heightmapy w 
        // prawo i w dół, więc omijam ostatni wiersz.
        for(int k = 0; k < width - 1; k++) {
            for(int m = 0; m < heightmapsInRow; m++) {
                TryToReadRowOfDataFromFile(heightmapsInRow, j, m);
            }
        }
    }
}

void HMParser::TryToReadRowOfDataFromFile(int heightmapsInRow, int rowNumber, int positionInRow) {
    try{
        short row[width];
        int index = heightmapsInRow * rowNumber + positionInRow;
        heightmapFiles[index].read(reinterpret_cast<char*>(&row), sizeof(short) * width);
        data.insert(data.end(), row, row + width - 1);
    } catch(const std::ifstream::failure &e) {
        std::cerr << "[EXCEPTION: HMParser::ParseHM] " << e.what() << '\n'
                  << "Error code: " << e.code() << '\n';
        exit(EXIT_FAILURE);
    } 
}

void HMParser::CloseFiles() {
    for(auto& hmFile: heightmapFiles)
        hmFile.close();
}

void HMParser::SwapBytesForAllValuesInHeightmap() {
    for(size_t i = 0; i < data.size(); i++) {
        data[i] = SwapBytes(data[i]);
    }
}

void HMParser::EliminateDataVoids() {
    for(int i = 0; i < totalWidth; i++) {
        for(int j = 0; j < totalWidth; j++) {
            int index = i * totalWidth + j;
            if(data[index] < -32000) {
                int count = 0;
                int sum = 0;
                int offset = 1;
                for(int k = i - offset; k <= i + offset; k++) {
                    for(int m = j - offset; m <= j + offset; m++) {
                        if(k == i && m == j) continue;
                        int index2 = k * totalWidth + m;
                        if(index2 > totalWidth * totalWidth || index2 < 0) continue;
                        if(data[index2] > -32000) {
                            sum += data[index2];
                            count++;
                        }
                    }
                }
                if(count > 0)
                    data[index] = sum / count;
                else data[index] = 0;
            }
        }
    }
}

std::vector<short>* HMParser::GetDataPtr() {
    return &data;
}

float HMParser::GetHeight(short h) {
    return (float)h / 50.0f;
}

glm::vec3 HMParser::GetPointTo(glm::vec3 point, PointTo_ pointTo) {
    int index;
    switch(pointTo) {
    case PointTo_Bottom:
        index = totalWidth * (point.z + 1) + point.x;
        return glm::vec3(point.x, heights[index], point.z + 1);
    case PointTo_BottomLeft:
        index = totalWidth * (point.z + 1) + (point.x - 1);
        return glm::vec3(point.x - 1, heights[index], point.z + 1);
    case PointTo_BottomRight:
        index = totalWidth * (point.z + 1) + point.x + 1;
        return glm::vec3(point.x + 1, heights[index], point.z + 1);
    case PointTo_Left:
        index = totalWidth * point.z + point.x - 1;
        return glm::vec3(point.x - 1, heights[index], point.z);
    case PointTo_Right:
        index = totalWidth * point.z + point.x + 1;
        return glm::vec3(point.x + 1, heights[index], point.z);
    case PointTo_Top:
        index = totalWidth * (point.z - 1) + point.x;
        return glm::vec3(point.x, heights[index], point.z - 1);
    case PointTo_TopLeft:
        index = totalWidth * (point.z - 1) + point.x - 1;
        return glm::vec3(point.x - 1, heights[index], point.z - 1);
    case PointTo_TopRight:
        index = totalWidth * (point.z - 1) + point.x + 1;
        return glm::vec3(point.x + 1, heights[index], point.z - 1);
    default:
        throw "Wrong direction";
    }
}

void HMParser::CalculateNormals() {
    for(int i = 0; i < totalWidth; i++) {
        for(int j = 0; j < totalWidth; j++) {
            glm::vec3 normal;
            glm::vec3 point(j, 0, i);
            float hL, hR, hU, hD;
            if(i == 0 && j == 0) { // top left corner
                hL = 0;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = 0;
                hD = GetPointTo(point, PointTo_Bottom).y;
            }  else if(i == 0 && j > 0 && j < totalWidth - 1) { // top edge
                hL = GetPointTo(point, PointTo_Left).y;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = 0;
                hD = GetPointTo(point, PointTo_Bottom).y;
            } else if(j == 0 && i > 0 && i < totalWidth - 1) { //left edge
                hL = 0;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = GetPointTo(point, PointTo_Top).y;
                hD = GetPointTo(point, PointTo_Bottom).y;
            } else if(i == totalWidth - 1 && j == totalWidth - 1) { // bottom right corner
                hL = GetPointTo(point, PointTo_Left).y;
                hR = 0;
                hU = GetPointTo(point, PointTo_Top).y;
                hD = 0;                
            } else if(i == totalWidth - 1 && j < totalWidth - 1 && j > 0) { // bottom edge
                hL = GetPointTo(point, PointTo_Left).y;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = GetPointTo(point, PointTo_Top).y;
                hD = 0;                
            } else if(i == totalWidth - 1 && j == 0) { // bottom left corner
                hL = 0;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = GetPointTo(point, PointTo_Top).y;
                hD = 0;                
            } else if(i == 0 && j == totalWidth - 1) { // top right corner
                hL = GetPointTo(point, PointTo_Left).y;
                hR = 0;
                hU = 0;
                hD = GetPointTo(point, PointTo_Bottom).y;                
            } else if(j == totalWidth - 1 && i > 0 && i < totalWidth - 1) { // top edge
                hL = GetPointTo(point, PointTo_Left).y;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = 0;
                hD = GetPointTo(point, PointTo_Bottom).y;                
            } else { // inside
                hL = GetPointTo(point, PointTo_Left).y;
                hR = GetPointTo(point, PointTo_Right).y;
                hU = GetPointTo(point, PointTo_Top).y;
                hD = GetPointTo(point, PointTo_Bottom).y;
            } 
            normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));            
            normals.push_back(normal.x * (normal.x < 0 ? 128 : 127));
            normals.push_back(normal.y * (normal.y < 0 ? 128 : 127));
            normals.push_back(normal.z * (normal.z < 0 ? 128 : 127));     
        }
    }
}

short HMParser::SwapBytes(short s) {
    return ((s & 0xff) << 8) | ((s & 0xff00) >> 8);
}

std::vector<char>* HMParser::GetNormalsPtr() {
    return &normals;
}

int HMParser::GetTotalWidth() {
    return totalWidth;
}
