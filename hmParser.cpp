#include <iostream>
#include <cmath>

#include <glm/glm.hpp>

#include "hmParser.hpp"


HMParser::HMParser(const std::vector<std::string>& heightmaps) 
: totalWidth((width - 1) * sqrt(heightmaps.size())) // z jakiegoś powodu ta inicjalizacja nie działa i muszę przypisać wartość jeszcze raz niżej
, heightmapFiles(heightmaps.size()) 
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
    CalculateNormals();
}

void HMParser::TryToOpenAFile(const std::string& path, std::ifstream& stream) {
    try {
        stream.open(path, std::ios_base::in | std::ios_base::binary);
    } catch(const std::ifstream::failure &e) {
        std::cerr << "[EXCEPTION: HMParser::ParseHeightmaps] " << e.what() << '\n'
                  << "Error code: " << e.code() << '\n'
                  << "In other words: Thre was a problem with opening a "
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
        data[i] = swapBytes(data[i]);
    }
}


std::vector<short>* HMParser::GetDataPtr() {
    return &data;
}


glm::vec3 HMParser::GetTriangleNormal(glm::vec3 point, glm::vec3 p1, glm::vec3 p2) {
    glm::vec3 edge1 = p1 - point;
    glm::vec3 edge2 = p2 - point;
    return glm::normalize(glm::cross(edge1, edge2));
}

static float GetHeight(short h) {
    return (float)h / 50.0f;
}

glm::vec3 HMParser::GetPointTo(glm::vec3 point, PointTo_ pointTo) {
    short pixel;
    switch(pointTo) {
    case PointTo_Bottom:
        pixel = data[totalWidth * (point.z + 1) + point.x];
        return glm::vec3(point.x, GetHeight(pixel), point.z + 1);
    case PointTo_BottomLeft:
        pixel = data[totalWidth * (point.z + 1) + (point.x - 1)];
        return glm::vec3(point.x - 1, GetHeight(pixel), point.z + 1);
    case PointTo_BottomRight:
        pixel = data[totalWidth * (point.z + 1) + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(pixel), point.z + 1);
    case PointTo_Left:
        pixel = data[totalWidth * point.z + point.x - 1];
        return glm::vec3(point.x - 1, GetHeight(pixel), point.z);
    case PointTo_Right:
        pixel = data[totalWidth * point.z + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(pixel), point.z);
    case PointTo_Top:
        pixel = data[totalWidth * (point.z - 1) + point.x];
        return glm::vec3(point.x, GetHeight(pixel), point.z - 1);
    case PointTo_TopLeft:
        pixel = data[totalWidth * (point.z - 1) + point.x - 1];
        return glm::vec3(point.x - 1, GetHeight(pixel), point.z - 1);
    case PointTo_TopRight:
        pixel = data[totalWidth * (point.z - 1) + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(pixel), point.z - 1);
    default:
        throw "Wrong direction";
    }
}

void HMParser::CalculateNormals() {
    std::vector< std::vector<glm::vec3> > triangleNormals = CalculateTriangleNormals();
    for(int i = 0; i < totalWidth * 2 - 1; i += 2) {
        for(int j = 0; j < totalWidth; j++) {
            glm::vec3 normal;
            if(i == 0 && j == 0) { // top left corner
                normal = triangleNormals[i][j];
            }  else if(i == 0 && j > 0 && j < totalWidth - 1) { // top edge
                glm::vec3 n1 = triangleNormals[i][j];
                glm::vec3 n2 = triangleNormals[i+1][j-1];
                glm::vec3 n3 = triangleNormals[i][j-1];
                normal = glm::normalize(n1 + n2 + n3); 
            } else if(j == 0 && i > 0 && i < totalWidth - 1) { //left edge
                glm::vec3 n1 = triangleNormals[i][j];
                glm::vec3 n2 = triangleNormals[i-1][j];
                glm::vec3 n3 = triangleNormals[i-2][j];
                normal = glm::normalize(n1 + n2 + n3); 
            } else if(i == totalWidth - 1 && j == totalWidth - 1) { // bottom right corner
                normal = triangleNormals[i][j-1];
            } else if(i == totalWidth - 1 && j < totalWidth - 1 && j > 0) { // bottom edge
                glm::vec3 n1 = triangleNormals[i-1][j];
                glm::vec3 n2 = triangleNormals[i][j-1];
                glm::vec3 n3 = triangleNormals[i][j];
                normal = glm::normalize(n1 + n2 + n3);
            } else if(i == totalWidth - 1 && j == 0) { // bottom left corner
                glm::vec3 n1 = triangleNormals[i][j];
                glm::vec3 n2 = triangleNormals[i-1][j];
                normal = glm::normalize(n1 + n2);
            } else if(i == 0 && j == totalWidth - 1) { // top right corner
                glm::vec3 n1 = triangleNormals[i][j-1];
                glm::vec3 n2 = triangleNormals[i+1][j-1];
                normal = glm::normalize(n1 + n2);
            } else if(j == totalWidth - 1 && i > 0 && i < totalWidth - 1) { // top edge
                glm::vec3 n1 = triangleNormals[i][j];
                glm::vec3 n2 = triangleNormals[i][j-1];
                glm::vec3 n3 = triangleNormals[i+1][j-1];
                normal = glm::normalize(n1 + n2 + n3);
            } else { // inside
                glm::vec3 n1 = triangleNormals[i][j];
                glm::vec3 n2 = triangleNormals[i-1][j];
                glm::vec3 n3 = triangleNormals[i-2][j];
                glm::vec3 n4 = triangleNormals[i-1][j-1];
                glm::vec3 n5 = triangleNormals[i][j-1];
                glm::vec3 n6 = triangleNormals[i+1][j-1];
                normal = glm::normalize(n1 + n2 + n3 + n4 + n5 + n6);
            } 
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);     
        }
    }
}

std::vector< std::vector<glm::vec3> > HMParser::CalculateTriangleNormals() {
    std::vector< std::vector<glm::vec3> > triangleNormals(totalWidth * 2, std::vector<glm::vec3>(totalWidth));
    for(int i = 0; i < totalWidth * 2 - 1; i += 2) {
        for(int j = 0; j < totalWidth - 1; j++) { 
            short pixel1 = data[totalWidth * (i / 2) + j];
            glm::vec3 p1 = glm::vec3(j, GetHeight(pixel1), i / 2);
            glm::vec3 p1Bot = GetPointTo(p1, PointTo_Bottom);
            glm::vec3 p1TopR = GetPointTo(p1, PointTo_Right);
            triangleNormals[i][j] = GetTriangleNormal(p1, p1Bot, p1TopR);
            
            short pixel2 = data[totalWidth * ((i / 2) + 1) + j];            
            glm::vec3 p2 = glm::vec3(j, GetHeight(pixel2), i / 2 + 1);
            glm::vec3 p2Right = GetPointTo(p2, PointTo_Right);
            glm::vec3 p2TopR = GetPointTo(p2, PointTo_TopRight);
            triangleNormals[i+1][j] = GetTriangleNormal(p2, p2Right, p2TopR);            
        }
    }
    return triangleNormals;
}

short HMParser::swapBytes(short s) {
    return ((s & 0xff) << 8) | ((s & 0xff00) >> 8);
}

std::vector<float>* HMParser::GetNormalsPtr() {
    return &normals;
}

int HMParser::GetTotalWidth() {
    return totalWidth;
}

float HMParser::GetHighestPoint() {
    return highestPoint;
}