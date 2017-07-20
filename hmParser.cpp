#include <cassert> //

#include <iostream>
#include <cmath>

#include "hmParser.hpp"


HMParser::HMParser(const std::vector<std::string>& heightmaps) 
: totalWidth(width * sqrt(heightmaps.size()))
, heightmapFiles(heightmaps.size()) 
, heightmapPaths(heightmaps) {
    int i = 0;
    for(auto& hmFile: heightmapFiles) {
        hmFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        TryToOpenAFile(heightmapPaths[i++], hmFile);
    }
    ParseHeightmaps();
    CloseFiles();
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