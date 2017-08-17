#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "lodPlane.hpp"
#include "tileGeometry.hpp"
#include "glDebug.hpp"
#include "tileMesh.hpp"
#include "hmParser.hpp"


LODPlane::LODPlane(const std::vector<std::string>& heightmapsPaths, int planeWidth,
                   const std::string& vshader, const std::string& fshader, 
                   const std::string& gshader, bool points) 
: xzOffset(0, 0)
, planeWidth(planeWidth)
, shader(vshader, fshader, gshader)
, points(points) {
    CalcLayersNumber();
    CreateTiles();
    SetUniforms();
    SetHeightmap(heightmapsPaths);

    unsigned char terrainColors[10 * 3] {
        79, 198, 79,
        59, 225, 59,
        150, 255, 0,
        210, 255, 0,
        255, 246, 0,
        255, 198, 0,
        255, 174, 0,
        255, 144, 0,
        255, 114, 0,
        255, 60, 0
    };
    shader.Uniform1i("terrainColors", 3);
    GL_CHECK(glGenTextures(1, &terrainColorsTex));
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, terrainColorsTex));
    GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CHECK(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, 10, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                          terrainColors));
}

LODPlane::~LODPlane() {
    GL_CHECK(glDeleteTextures(1, &heightmapTex));
    GL_CHECK(glDeleteTextures(1, &normalMapTex));
    GL_CHECK(glDeleteTextures(1, &terrainColorsTex));
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

void LODPlane::SetUniforms() {
    shader.Uniform1i("tileSize", TileGeometry::GetInstance()->tileSize);
    shader.Uniform1f("morphRegion", morphRegion);
    shader.UniformMatrix4fv("projMat", projectionMatrix);
    shader.Uniform1i("meshSize", planeWidth);
}

void LODPlane::SetHeightmap(const std::vector<std::string>& heightmapsPaths) {
    shader.Uniform1i("heightmap", 0);
    GL_CHECK(glGenTextures(1, &heightmapTex));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightmapTex));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    // GL_CHECK(glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_TRUE));

    HMParser hmParser(heightmapsPaths);
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, hmParser.GetTotalWidth(), 
                          hmParser.GetTotalWidth(), 0, GL_RED_INTEGER, GL_SHORT, 
                          hmParser.GetDataPtr()->data()));
    
    // if(points) return;
    
    shader.Uniform1i("normalMap", 2);
    GL_CHECK(glGenTextures(1, &normalMapTex));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, normalMapTex));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    GL_CHECK(glPixelStorei(GL_UNPACK_SWAP_BYTES, GL_FALSE));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hmParser.GetTotalWidth(), 
                          hmParser.GetTotalWidth(), 0, GL_RGB, GL_FLOAT, 
                          hmParser.GetNormalsPtr()->data()));
    
    shader.Uniform1f("highestPoint", hmParser.GetHighestPoint());
}

void LODPlane::DrawFrom(const MainCamera &camera, const Camera* additionalCam) const {
    if(!meshMovementLocked)
        shader.Uniform2f("globalOffset", camera.GetPosition().x, camera.GetPosition().z);

    GL_CHECK(glBindVertexArray(TileGeometry::GetInstance()->GetVaoId()));

    glm::mat4 viewMat;
    if(additionalCam) viewMat = additionalCam->GetViewMatrix();
    else viewMat = camera.GetViewMatrix();

    shader.UniformMatrix4fv("viewMat", viewMat);

    for(unsigned int i = 0; i < tiles.size(); i++) {
        shader.Uniform1i("level", i);
        for(unsigned int j = 0; j < tiles[i].size(); j++) {
            if(IsTileInsideFrustum(i, j, camera)) {
                shader.Uniform2f("localOffset", tiles[i][j].GetLocalOffset().x, 
                                 tiles[i][j].GetLocalOffset().y);
                shader.Uniform1i("edgeMorph", tiles[i][j].GetEdgeMorph());
                
                shader.Use();
                GL_CHECK(glActiveTexture(GL_TEXTURE0));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, heightmapTex));
                GL_CHECK(glActiveTexture(GL_TEXTURE2));
                GL_CHECK(glBindTexture(GL_TEXTURE_2D, normalMapTex));
                GL_CHECK(glActiveTexture(GL_TEXTURE3));
                GL_CHECK(glBindTexture(GL_TEXTURE_1D, terrainColorsTex));
                
                GL_CHECK(glDrawElements(points ? GL_POINTS : GL_TRIANGLE_STRIP, 
                    TileGeometry::GetInstance()->GetIndicesSize(), GL_UNSIGNED_INT, 
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

GLuint LODPlane::GetHeightmapTexture() const {
    return heightmapTex;
}

void LODPlane::ToggleMeshMovementLock(MainCamera &mainCam) {
    meshMovementLocked = !meshMovementLocked;
    mainCam.SetMeshMovementLock(meshMovementLocked);
}

GLuint LODPlane::GetNormalMapTex() {
    return normalMapTex;
}

GLuint LODPlane::GetHeightmapTex() {
    return heightmapTex;
}