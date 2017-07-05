#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "mainCamera.hpp"
#include "edgeMorph.hpp"
#include "tileMesh.hpp"

using namespace std;


class LODPlane {
private:
    GLuint heightmapTex;
    GLuint unifHeightmapOffset;
    vector< vector<TileMesh> > tiles;
    int layers;
    int lodMeshWidth; 

    void CalcLayersNumber();
    void CreateTiles();
    bool IsTileInsideCameraView(int i, int j, const MainCamera &camera);
    bool IsTileInsideFrustum(int i, int j, const MainCamera &mainCam) const;

public:
    static const int planeWidth = 1024;
    static constexpr float morphRegion = 0.3f;
    
    LODPlane();
    ~LODPlane();

    void SetHeightmap(vector<short>*);
    void DrawFrom(const MainCamera &camera, const Camera* additionalCam = nullptr) const;
    GLuint GetHeightmapTexture() const;
};
