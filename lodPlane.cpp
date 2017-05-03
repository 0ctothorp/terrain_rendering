#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb/stb_image.h"

#include "lodPlane.hpp"
#include "tileGeometry.hpp"
#include "tileMaterial.hpp"
#include "glDebug.hpp"
#include "tileMesh.hpp"


LODPlane::LODPlane(int windowW, int windowH, Camera *_camera) 
: camera(_camera) {
    CalcLayersNumber();
    CreateTiles();
    // TileMesh::globalOffset = glm::vec2(planeWidth / 2, planeWidth / 2);
    GL_CHECK(
        glUniform2f(
            glGetUniformLocation(TileMaterial::shader->GetProgramId(), "globalOffset"), 
            TileMesh::globalOffset.x, TileMesh::globalOffset.y
        )
    );    
}

LODPlane::~LODPlane() {
    GL_CHECK(glDeleteTextures(1, &heightmapTex));
    GL_CHECK(glDeleteTextures(1, &testTex));
}

void LODPlane::CalcLayersNumber() {
    int zeroLevel = 4 * TileGeometry::GetInstance()->tileSize;
    layers = 1;
    while(zeroLevel * 2 <= planeWidth) {
        zeroLevel *= 2;
        layers++;
    }
    lodMeshWidth = zeroLevel;
}

void LODPlane::CreateTiles() {
    tiles.resize(layers);
    tiles[0].push_back(TileMesh(glm::vec2(-1, -1), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(-1, 0), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, -1), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, 0), EdgeMorph::NONE));

    for(int layer = 0; layer < layers; layer++) {
        // Left strip
        int scale = pow(2, layer);
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, -scale * 2), EdgeMorph::TOP | EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, -scale), EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, 0), EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, scale), EdgeMorph::LEFT | EdgeMorph::BOTTOM));

        // Middle-left strip
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale, -scale * 2), EdgeMorph::TOP));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale, scale), EdgeMorph::BOTTOM));

        // Middle-right strip
        tiles[layer].push_back(TileMesh(glm::vec2(0, -scale * 2),  EdgeMorph::TOP));
        tiles[layer].push_back(TileMesh(glm::vec2(0, scale),  EdgeMorph::BOTTOM));

        // Right strip
        tiles[layer].push_back(
            TileMesh(glm::vec2(scale, -scale * 2), EdgeMorph::TOP | EdgeMorph::RIGHT));
        tiles[layer].push_back(TileMesh(glm::vec2(scale, -scale), EdgeMorph::RIGHT));
        tiles[layer].push_back(TileMesh(glm::vec2(scale, 0), EdgeMorph::RIGHT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(scale, scale), EdgeMorph::RIGHT | EdgeMorph::BOTTOM));
    }
}

void LODPlane::Draw() {
    GL_CHECK(glUseProgram(TileMaterial::shader->GetProgramId()));
    GL_CHECK(glBindVertexArray(TileGeometry::GetInstance()->GetVaoId()));
    glm::mat4 viewMat = camera->GetViewMatrix();
    GL_CHECK(glUniformMatrix4fv(TileMaterial::GetUnifViewMat(), 1, GL_FALSE, 
                                glm::value_ptr(viewMat)));
    int indicesSize = TileGeometry::GetInstance()->GetIndicesSize();

    for(int i = 0; i < tiles.size(); i++) {
        glUniform1i(tiles[i][0].material.GetUnifLevel(), i);
        for(int j = 0; j < tiles[i].size(); j++) {
            GL_CHECK(glUniform2f(tiles[i][j].material.GetUnifLocOffset(), 
                                 tiles[i][j].GetLocalOffset().x, tiles[i][j].GetLocalOffset().y));
            GL_CHECK(glUniform1i(tiles[i][j].material.GetUnifEdgeMorph(), 
                                 tiles[i][j].GetEdgeMorph()));
            GL_CHECK(glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 
                                    TileGeometry::GetInstance()->GetIndicesBufferPtr()));
        }
    }
    GL_CHECK(glBindVertexArray(0));
}

void LODPlane::SetHeightmap(vector<short>* hmData) {
    Shader* shader = TileMaterial::shader;
    if(shader == nullptr) throw "Static field shader not initialized.";
    GL_CHECK(glUseProgram(shader->GetProgramId()));
    GL_CHECK(glUniform1i(glGetUniformLocation(shader->GetProgramId(), "heightmap"), 0));

    GL_CHECK(glGenTextures(1, &heightmapTex));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightmapTex));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, 1201, 1201, 0, GL_RED_INTEGER, GL_SHORT, 
                          hmData->data()));

    // // debug texture
    // // for some reason initializing it in LODPlane constructor gives black texture in a shader
    // int x, y, n;
    // unsigned char *imgData = stbi_load("textures/test.jpg", &x, &y, &n, 0);
    // if(imgData == NULL) throw "couldn't load textures/test.jpg";
    // // ... process data if not NULL ...
    // // ... x = width, y = height, n = # 8-bit components per pixel ...
    // // ... replace '0' with '1'..'4' to force that many components per pixel
    // // ... but 'n' will always be the number that it would have been if you said 0
    // GL_CHECK(glUniform1i(glGetUniformLocation(shader->GetProgramId(), "testTex"), 1));

    // GL_CHECK(glGenTextures(1, &testTex));
    // GL_CHECK(glActiveTexture(GL_TEXTURE0 + 1));
    // GL_CHECK(glBindTexture(GL_TEXTURE_2D, testTex));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    // GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    // GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, 
    //                       imgData));
    // stbi_image_free(imgData);
}
