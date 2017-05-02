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
    // static unique_ptr<LODPlane> instance;
    GLuint heightmapTex;
    GLuint testTex;

    vector< vector<TileMesh> > tiles;
    int layers;
    int lodMeshWidth;  
    Camera *camera;

    void CalcLayersNumber();
    void CreateTiles();

public:
    static const int planeWidth = 1024;
    static constexpr float morphRegion = 0.3f;
    
    LODPlane(int windowW, int windowH, Camera *camera = nullptr);
    ~LODPlane();

    void SetHeightmap(vector<short>*);
    void Draw();
};
