#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mainCamera.hpp"
#include "edgeMorph.hpp"
#include "tileMesh.hpp"
#include "shader.hpp"


class LODPlane {
private:
    GLuint heightmapTex;
    GLuint normalMapTex;
    GLuint terrainColorsTex;
    std::vector< std::vector<TileMesh> > tiles;
    int layers;
    glm::vec2 xzOffset;
    int planeWidth;
    bool meshMovementLocked = false;
    bool points = false;

    void CalcLayersNumber();
    void CreateTiles();
    bool IsTileInsideFrustum(int i, int j, const MainCamera &mainCam) const;
    void SetHeightmap(const std::vector<std::string>& heightmapsPaths);
    void SetUniforms();

public:
    const float morphRegion = 0.3f;
    const Shader shader;
    const glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(Camera::fov), 
        (float)Window::width / (float)Window::height,
        0.01f,
        5000.0f
    );
    
    LODPlane(const std::vector<std::string>& heightmapsPaths, int planeWidth,
             const std::string& vshader, const std::string& fshader, 
             const std::string& gshader, bool points = false);
    ~LODPlane();

    void DrawFrom(const MainCamera &camera, const Camera* additionalCam = nullptr) const;
    GLuint GetHeightmapTexture() const;
    void ToggleMeshMovementLock(MainCamera &mainCam);
    GLuint GetNormalMapTex();
    GLuint GetHeightmapTex();
};
