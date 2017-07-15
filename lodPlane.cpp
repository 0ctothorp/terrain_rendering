#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "lodPlane.hpp"
#include "tileGeometry.hpp"
#include "glDebug.hpp"
#include "tileMesh.hpp"
#include "hmParser.hpp"


LODPlane::LODPlane() 
: shader("shaders/planeVertexShader.glsl", "shaders/planeFragmentShader.glsl") {
    CalcLayersNumber();
    CreateTiles();
    shader.Use();
    GL_CHECK(glUniform1i(shader.GetUniform("tileSize"), TileGeometry::GetInstance()->tileSize));
    GL_CHECK(glUniform1f(shader.GetUniform("morphRegion"), morphRegion));
    GL_CHECK(glUniform2f(shader.GetUniform("globalOffset"), 
                         TileMesh::GetGlobalOffset().x, TileMesh::GetGlobalOffset().y));
    GL_CHECK(glUniformMatrix4fv(shader.GetUniform("projMat"), 1, GL_FALSE, 
                                glm::value_ptr(projectionMatrix)));
    GL_CHECK(glUniform1i(shader.GetUniform("meshSize"), LODPlane::planeWidth));
    SetHeightmap();
}

LODPlane::~LODPlane() {
    GL_CHECK(glDeleteTextures(1, &heightmapTex));
}

void LODPlane::CalcLayersNumber() {
    int zeroLevel = 4 * TileGeometry::GetInstance()->tileSize;
    layers = 1;
    while(zeroLevel * 2 <= planeWidth) {
        zeroLevel *= 2;
        layers++;
    }
}

void LODPlane::CreateTiles() {
    tiles.resize(layers);
    tiles[0].push_back(TileMesh(glm::vec2(-1, -1), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(-1, 0), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, -1), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, 0), EdgeMorph::NONE));

    for(int layer = 0; layer < layers; layer++) {
        int scale = pow(2, layer);
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, -scale * 2), EdgeMorph::TOP | EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, -scale), EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, 0), EdgeMorph::LEFT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale * 2, scale), EdgeMorph::LEFT | EdgeMorph::BOTTOM));

        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale, -scale * 2), EdgeMorph::TOP));
        tiles[layer].push_back(
            TileMesh(glm::vec2(-scale, scale), EdgeMorph::BOTTOM));

        tiles[layer].push_back(TileMesh(glm::vec2(0, -scale * 2),  EdgeMorph::TOP));
        tiles[layer].push_back(TileMesh(glm::vec2(0, scale),  EdgeMorph::BOTTOM));

        tiles[layer].push_back(
            TileMesh(glm::vec2(scale, -scale * 2), EdgeMorph::TOP | EdgeMorph::RIGHT));
        tiles[layer].push_back(TileMesh(glm::vec2(scale, -scale), EdgeMorph::RIGHT));
        tiles[layer].push_back(TileMesh(glm::vec2(scale, 0), EdgeMorph::RIGHT));
        tiles[layer].push_back(
            TileMesh(glm::vec2(scale, scale), EdgeMorph::RIGHT | EdgeMorph::BOTTOM));
    }
}

void LODPlane::DrawFrom(const MainCamera &camera, const Camera* additionalCam) const {
    shader.Use();
    GL_CHECK(glBindVertexArray(TileGeometry::GetInstance()->GetVaoId()));
    glm::mat4 viewMat;
    if(additionalCam) viewMat = additionalCam->GetViewMatrix();
    else viewMat = camera.GetViewMatrix();
    GL_CHECK(glUniformMatrix4fv(shader.GetUniform("viewMat"), 1, GL_FALSE, 
                                glm::value_ptr(viewMat)));
    int indicesSize = TileGeometry::GetInstance()->GetIndicesSize();

    for(unsigned int i = 0; i < tiles.size(); i++) {
        glUniform1i(shader.GetUniform("level"), i);
        for(unsigned int j = 0; j < tiles[i].size(); j++) {
            if(IsTileInsideFrustum(i, j, camera)) {
                GL_CHECK(glUniform2f(shader.GetUniform("localOffset"), 
                     tiles[i][j].GetLocalOffset().x, tiles[i][j].GetLocalOffset().y));
                GL_CHECK(glUniform1i(shader.GetUniform("edgeMorph"), 
                    tiles[i][j].GetEdgeMorph()));
                GL_CHECK(glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 
                    TileGeometry::GetInstance()->GetIndicesBufferPtr()));
            } 
        }
    }
}

bool LODPlane::IsTileInsideFrustum(int i, int j, const MainCamera &mainCam) const {
    glm::vec2 upperLeft = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize + 
        TileMesh::GetGlobalOffset();
    glm::vec2 upperRight = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize
        + glm::vec2((float)TileGeometry::tileSize * pow(2, i), 0.f) + TileMesh::GetGlobalOffset();     
    glm::vec2 lowerRight = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize 
                           + glm::vec2((float)TileGeometry::tileSize * pow(2, i),
                                       (float)TileGeometry::tileSize * pow(2, i)) + 
                           TileMesh::GetGlobalOffset();
    glm::vec2 lowerLeft = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize 
                          + glm::vec2(0.f, (float)TileGeometry::tileSize * pow(2, i)) + 
                          TileMesh::GetGlobalOffset(); 

    glm::vec3 upLeft = glm::vec3(upperLeft.x, 0.0f, upperLeft.y);
    glm::vec3 upRight = glm::vec3(upperRight.x, 0.0f, upperRight.y);
    glm::vec3 loRight = glm::vec3(lowerRight.x, 0.0f, lowerRight.y);
    glm::vec3 loLeft = glm::vec3(lowerLeft.x, 0.0f, lowerLeft.y);

    return mainCam.IsInsideFrustum(upLeft, upRight, loRight, loLeft);
}

void LODPlane::SetHeightmap() {
    shader.Use();
    GL_CHECK(glUniform1i(shader.GetUniform("heightmap"), 0));

    GL_CHECK(glGenTextures(1, &heightmapTex));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightmapTex));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE));

    HMParser hmParser("heightmaps/N50E016.hgt");
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, 1201, 1201, 0, GL_RED, GL_SHORT, 
                          hmParser.GetDataPtr()->data()));
}

GLuint LODPlane::GetHeightmapTexture() const {
    return heightmapTex;
}