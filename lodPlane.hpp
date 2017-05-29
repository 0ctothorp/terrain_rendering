#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>

#include "camera.hpp"
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
    bool IsTileInsideCameraView(int i, int j, Camera *camera);

public:
    static const int planeWidth = 1024;
    static constexpr float morphRegion = 0.3f;
    
    LODPlane();
    ~LODPlane();

    void SetHeightmap(vector<short>*);
    void Draw(Camera *camera);
};
