#pragma once

#include <string>

#include "libs/stb/stb_image.h"


class Img {
private:
    unsigned char* data;
    int sizeX, sizeY, channels;
public:
    Img(std::string filePath, int request_components=0) {
        data = stbi_load(filePath.c_str(), &sizeX, &sizeY, &channels, request_components);
    }

    ~Img()                   { stbi_image_free(data); }
    int GetSizeX()           { return sizeX; }
    int GetSizeY()           { return sizeY; }
    int GetChannels()        { return channels; }
    unsigned char* GetData() { return data; }
};