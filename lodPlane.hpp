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
    Camera *camera;
    float heightmapOffsetX = 0;
    float heightmapOffsetY = 0;

    void CalcLayersNumber();
    void CreateTiles();

public:
    static const int planeWidth = 1024;
    static constexpr float morphRegion = 0.3f;
    
    LODPlane(int windowW, int windowH, Camera *camera);
    ~LODPlane();

    void SetHeightmap(vector<short>*);
    void Draw();
};
