#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "mainCamera.hpp"
#include "edgeMorph.hpp"
#include "tileMesh.hpp"
#include "shader.hpp"

using namespace std;


class LODPlane {
private:
    GLuint heightmapTex;
    vector< vector<TileMesh> > tiles;
    int layers;

    void CalcLayersNumber();
    void CreateTiles();
    bool IsTileInsideCameraView(int i, int j, const MainCamera &camera);
    bool IsTileInsideFrustum(int i, int j, const MainCamera &mainCam);

public:
    static const int planeWidth = 1024;
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

    void SetHeightmap(vector<short>*);
    void DrawFrom(const MainCamera &camera, const Camera* additionalCam = nullptr);
    GLuint GetHeightmapTexture();
};
