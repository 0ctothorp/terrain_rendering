#include <iostream>
#include <cmath>

#include <glm/glm.hpp>

#include "hmParser.hpp"


HMParser::HMParser(const std::vector<std::string>& heightmaps) 
: totalWidth((width - 1) * sqrt(heightmaps.size())) // z jakiegoś powodu ta inicjalizacja nie działa
, heightmapFiles(heightmaps.size()) 
, heightmapPaths(heightmaps) {
    int i = 0;
    for(auto& hmFile: heightmapFiles) {
        hmFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        TryToOpenAFile(heightmapPaths[i++], hmFile);
    }
    ParseHeightmaps();
    CloseFiles();
    std::cerr << "totalWidth: " << totalWidth << "\n";
    totalWidth = (width - 1) * sqrt(heightmaps.size());
    std::cerr << "totalWidth: " << totalWidth << "\n";    
    CalculateNormals();
    // for(int i = 0; i < normals.size(); i++) {
    //     std::cout << (int)normals[i] << ", ";
    // }
    // std::cout << std::endl;
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
                TryToReadAFile(heightmapsInRow, j, m);
            }
        }
    }
}

void HMParser::TryToReadAFile(int heightmapsInRow, int rowNumber, int positionInRow) {
    try{
        short row[width];
        heightmapFiles[heightmapsInRow * rowNumber + positionInRow]
            .read(reinterpret_cast<char*>(&row), sizeof(short) * width);
        // width - 1, bo z takiego powodu, jak opisałem w HMParser::ParseHeightmaps
        // omijam ostatnią kolumnę danych.
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

std::vector<short>* HMParser::GetDataPtr() {
    return &data;
}


glm::vec3 HMParser::GetTriangleNormal(glm::vec3 point, glm::vec3 p1, glm::vec3 p2) {
    glm::vec3 edge1 = p1 - point;
    glm::vec3 edge2 = p2 - point;
    return glm::normalize(glm::cross(edge1, edge2));
}

static float GetHeight(short h) {
    // return (((float)h / 32767.0f + 1.0f) / 2.0f) * 750.0f;
    return h;
}

glm::vec3 HMParser::GetPointTo(glm::vec3 point, PointTo_ pointTo) {
    switch(pointTo) {
    case PointTo_Bottom: {
        short pixel = data[totalWidth * (point.z + 1) + point.x];
        return glm::vec3(point.x, GetHeight(swapBytes(pixel)), point.z + 1);
    }
    case PointTo_BottomLeft: {
        short pixel = data[totalWidth * (point.z + 1) + (point.x - 1)];
        return glm::vec3(point.x - 1, GetHeight(swapBytes(pixel)), point.z + 1);
    }
    case PointTo_BottomRight: {
        short pixel = data[totalWidth * (point.z + 1) + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(swapBytes(pixel)), point.z + 1);
    }
    case PointTo_Left: {
        short pixel = data[totalWidth * point.z + point.x - 1];
        return glm::vec3(point.x - 1, GetHeight(swapBytes(pixel)), point.z);
    }
    case PointTo_Right: {
        short pixel = data[totalWidth * point.z + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(swapBytes(pixel)), point.z);
    }
    case PointTo_Top: {
        short pixel = data[totalWidth * (point.z - 1) + point.x];
        return glm::vec3(point.x, GetHeight(swapBytes(pixel)), point.z - 1);
    }
    case PointTo_TopLeft: {
        short pixel = data[totalWidth * (point.z - 1) + point.x - 1];
        return glm::vec3(point.x - 1, GetHeight(swapBytes(pixel)), point.z - 1);
    }
    case PointTo_TopRight: {
        short pixel = data[totalWidth * (point.z - 1) + point.x + 1];
        return glm::vec3(point.x + 1, GetHeight(swapBytes(pixel)), point.z - 1);
    }
    default:
        break;
    }
}

glm::vec3 HMParser::GetTriangleNormalTo(glm::vec3 point, PointTo_ dir) {
    glm::vec3 point1, point2;
    switch(dir) {
    case PointTo_Bottom:
        point1 = GetPointTo(point, PointTo_Bottom);
        point2 = GetPointTo(point, PointTo_BottomLeft); break;
    case PointTo_BottomLeft:
        point1 = GetPointTo(point, PointTo_Left);
        point2 = GetPointTo(point, PointTo_BottomLeft); break;
    case PointTo_BottomRight:
        point1 = GetPointTo(point, PointTo_Bottom);
        point2 = GetPointTo(point, PointTo_Right); break;
    case PointTo_TopRight:
        point1 = GetPointTo(point, PointTo_Right);
        point2 = GetPointTo(point, PointTo_TopRight); break;
    case PointTo_TopLeft:
        point1 = GetPointTo(point, PointTo_Top);
        point2 = GetPointTo(point, PointTo_Left); break;
    case PointTo_Top:
        point1 = GetPointTo(point, PointTo_Top);
        point2 = GetPointTo(point, PointTo_TopRight); break;
    default:
        throw "Wrong direction";
    }
    return GetTriangleNormal(point, point1, point2);
}

void HMParser::CalculateNormals() {
    int step = (totalWidth) / 512.0f;
    for(int i = 0; i < totalWidth; i += step) {
        for(int j = 0; j < totalWidth; j += step) {
            short pixel = data[totalWidth * i + j];
            glm::vec3 point = glm::vec3(i, GetHeight(swapBytes(pixel)), j);
            // std::cout << point.x << ", " << point.y << ", " << point.z << std::endl;
            glm::vec3 normal;
            if(i == 0 && j == 0) {
                normal = GetTriangleNormalTo(point, PointTo_BottomRight);
            }  else if(i == 0 && j > 0 && j < totalWidth - 1) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Top);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_TopRight);
                glm::vec3 n3 = GetTriangleNormalTo(point, PointTo_BottomRight);
                normal = glm::normalize(n1 + n2 + n3); 
            } else if(j == 0 && i > 0 && i < totalWidth - 1) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Bottom);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_BottomLeft);
                glm::vec3 n3 = GetTriangleNormalTo(point, PointTo_BottomRight);
                normal = glm::normalize(n1 + n2 + n3); 
            } else if(i == totalWidth - 1 && j == totalWidth - 1) {
                normal = GetTriangleNormalTo(point, PointTo_TopLeft);
            } else if(i == totalWidth - 1 && j < totalWidth - 1 && j > 0) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Bottom);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_BottomLeft);
                glm::vec3 n3 = GetTriangleNormalTo(point, PointTo_TopLeft);
                normal = glm::normalize(n1 + n2 + n3);
            } else if(i == totalWidth - 1 && j == 0) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Bottom);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_BottomLeft);
                normal = glm::normalize(n1 + n2);
            } else if(i == 0 && j == totalWidth - 1) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Top);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_TopRight);
                normal = glm::normalize(n1 + n2);
            } else if(j == totalWidth - 1 && i > 0 && i < totalWidth - 1) {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_TopLeft);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_Top);
                glm::vec3 n3 = GetTriangleNormalTo(point, PointTo_TopRight);
                normal = glm::normalize(n1 + n2 + n3);
            } else {
                glm::vec3 n1 = GetTriangleNormalTo(point, PointTo_Top);
                glm::vec3 n2 = GetTriangleNormalTo(point, PointTo_TopRight);
                glm::vec3 n3 = GetTriangleNormalTo(point, PointTo_BottomRight);
                glm::vec3 n4 = GetTriangleNormalTo(point, PointTo_Bottom);
                glm::vec3 n5 = GetTriangleNormalTo(point, PointTo_BottomLeft);
                glm::vec3 n6 = GetTriangleNormalTo(point, PointTo_TopLeft);
                normal = glm::normalize(n1 + n2 + n3 + n4 + n5 + n6);
            } 
            // std::cout << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
            normals.push_back(normal.x);
            normals.push_back(normal.y);
            normals.push_back(normal.z);
            // std::cout << normals[(i * totalWidth + j) * 3] << ", " << normals[(i * totalWidth + j) * 3 + 1] 
            //           << ", " << normals[(i * totalWidth + j) * 3 + 2] << std::endl;            
        }
    }
}

short HMParser::swapBytes(short s) {
    return (s << 8) | (s >> 8);
}

std::vector<float>* HMParser::GetNormalsPtr() {
    return &normals;
}

int HMParser::GetTotalWidth() {
    return totalWidth;
}