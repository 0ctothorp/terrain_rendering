#pragma once

#include <vector>

#include <GL/glew.h>

#include "camera.hpp"
#include "edgeMorph.hpp"
#include "tileMesh.hpp"

using namespace std;


class LODPlane {
private:
    vector< vector<TileMesh> > tiles;
    const int planeWidth = 512;
    int layers;
    int lodMeshWidth;  
    Camera *camera;

    void CalcLayersNumber();
    void CreateTiles();

public:
    LODPlane(int windowW, int windowH, Camera *camera = nullptr);
    void Draw();
};