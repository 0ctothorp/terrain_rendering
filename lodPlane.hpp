#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "mainCamera.hpp"
#include "edgeMorph.hpp"
#include "tileMesh.hpp"
#include "shader.hpp"


class LODPlane {
private:
    GLuint heightmapTex;
    std::vector< std::vector<TileMesh> > tiles;
    int layers;
    glm::vec2 xzOffset;

    void CalcLayersNumber();
    void CreateTiles();
    bool IsTileInsideFrustum(int i, int j, const MainCamera &mainCam) const;
    void SetHeightmap();
    void SetUniforms();

public:
    static const int planeWidth = 2048;
    const float morphRegion = 0.3f;
    const Shader shader;
    const glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(Camera::fov), 
        (float)Window::width / (float)Window::height,
        0.01f,
        2000.0f
    );
    
    LODPlane();
    ~LODPlane();

    void DrawFrom(const MainCamera &camera, const Camera* additionalCam = nullptr) const;
    GLuint GetHeightmapTexture() const;
};
