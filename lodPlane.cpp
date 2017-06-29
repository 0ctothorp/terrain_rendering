#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "lodPlane.hpp"
#include "tileGeometry.hpp"
#include "tileMaterial.hpp"
#include "glDebug.hpp"
#include "tileMesh.hpp"


LODPlane::LODPlane() {
    CalcLayersNumber();
    CreateTiles();
    GL_CHECK(glUniform2f(
        TileMaterial::GetUnifGlobOffset(), TileMesh::globalOffset.x, TileMesh::globalOffset.y
    ));
    unifHeightmapOffset = TileMaterial::shader->GetUniformLocation("heightmapOffset");
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
    lodMeshWidth = zeroLevel;
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

void LODPlane::DrawFrom(const MainCamera &camera, const Camera* additionalCam) {
    TileMaterial::shader->Use();
    GL_CHECK(glBindVertexArray(TileGeometry::GetInstance()->GetVaoId()));
    glm::mat4 viewMat;
    if(additionalCam) viewMat = additionalCam->GetViewMatrix();
    else viewMat = camera.GetViewMatrix();
    GL_CHECK(glUniformMatrix4fv(TileMaterial::GetUnifViewMat(), 1, GL_FALSE, 
                                glm::value_ptr(viewMat)));
    int indicesSize = TileGeometry::GetInstance()->GetIndicesSize();

    for(int i = 0; i < tiles.size(); i++) {
        glUniform1i(tiles[i][0].material.GetUnifLevel(), i);
        for(int j = 0; j < tiles[i].size(); j++) {
            if(IsTileInsideCameraView(i, j, camera)) {
                GL_CHECK(glUniform2f(tiles[i][j].material.GetUnifLocOffset(), 
                                     tiles[i][j].GetLocalOffset().x, 
                                     tiles[i][j].GetLocalOffset().y));
                GL_CHECK(glUniform1i(tiles[i][j].material.GetUnifEdgeMorph(), 
                                     tiles[i][j].GetEdgeMorph()));
                GL_CHECK(glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 
                                        TileGeometry::GetInstance()->GetIndicesBufferPtr()));
            } 
        }
    }
    GL_CHECK(glBindVertexArray(0));
}

// Sprawdzanie, czy dany kafelek terenu należy narysować. Wyliczam kąt między wektorem 
// ograniczającym widok z lewej oraz wektorem prowadzącym do punktu kafelka(w sumie do 4), 
// a następnie wyliczam kąt między wektorem ograniczającym widok z prawej, a wektorem 
// do punktu kafelka. Razem te kąty mają dać fov.
bool LODPlane::IsTileInsideCameraView(int i, int j, const MainCamera &camera) {
    glm::vec2 upperLeftTileCornerPos = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize;
    glm::vec2 upperRightTileCornerPos = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize
                                        + glm::vec2((float)TileGeometry::tileSize * pow(2, i), 0.f);     
    glm::vec2 lowerRightTileCornerPos = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize 
                                        + glm::vec2((float)TileGeometry::tileSize * pow(2, i),
                                                    (float)TileGeometry::tileSize * pow(2, i));
    glm::vec2 lowerLeftTileCornerPos = tiles[i][j].GetLocalOffset() * (float)TileGeometry::tileSize 
                                       + glm::vec2(0.f, (float)TileGeometry::tileSize * pow(2, i)); 
    glm::vec2 tilePointingVector = glm::normalize(upperLeftTileCornerPos);
    glm::vec2 tilePointingVector2 = glm::normalize(upperRightTileCornerPos);
    glm::vec2 tilePointingVector3 = glm::normalize(lowerRightTileCornerPos);
    glm::vec2 tilePointingVector4 = glm::normalize(lowerLeftTileCornerPos);
    float leftAngle = acos(glm::dot(camera.GetLeftViewLimit(), tilePointingVector));
    float rightAngle = acos(glm::dot(tilePointingVector, camera.GetRightViewLimit()));
    float leftAngle2 = acos(glm::dot(camera.GetLeftViewLimit(), tilePointingVector2));
    float rightAngle2 = acos(glm::dot(tilePointingVector2, camera.GetRightViewLimit()));
    float leftAngle3 = acos(glm::dot(camera.GetLeftViewLimit(), tilePointingVector3));
    float rightAngle3 = acos(glm::dot(tilePointingVector3, camera.GetRightViewLimit()));
    float leftAngle4 = acos(glm::dot(camera.GetLeftViewLimit(), tilePointingVector4));
    float rightAngle4 = acos(glm::dot(tilePointingVector4, camera.GetRightViewLimit()));
    return (leftAngle + rightAngle > glm::radians(Camera::fov - 1) && leftAngle + 
            rightAngle < glm::radians(Camera::fov + 1)) ||
           (leftAngle2 + rightAngle2 > glm::radians(Camera::fov - 1) && leftAngle2 + 
            rightAngle2 < glm::radians(Camera::fov + 1)) ||
           (leftAngle3 + rightAngle3 > glm::radians(Camera::fov - 1) && leftAngle3 + 
            rightAngle3 < glm::radians(Camera::fov + 1)) || 
           (leftAngle4 + rightAngle4 > glm::radians(Camera::fov - 1) && leftAngle4 + 
            rightAngle4 < glm::radians(Camera::fov + 1));
}

void LODPlane::SetHeightmap(vector<short>* hmData) {
    Shader* shader = TileMaterial::shader;
    if(!shader) throw "Static field shader not initialized.";
    shader->Use();
    GL_CHECK(glUniform1i(shader->GetUniformLocation("heightmap"), 0));

    GL_CHECK(glGenTextures(1, &heightmapTex));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightmapTex));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, 1201, 1201, 0, GL_RED, GL_SHORT, 
                          hmData->data()));
}

GLuint LODPlane::GetHeightmapTexture() {
    return heightmapTex;
}