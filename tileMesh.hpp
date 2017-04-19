#pragma once

#include <glm/glm.hpp>

#include "tileGeometry.hpp"
#include "tileMaterial.hpp"
#include "edgeMorph.hpp"


class TileMesh {
private:
    static TileGeometry* tileGeom;
    glm::vec2 localOffset;
    int edgeMorph;
public:
    TileMaterial material;
    static glm::vec2 globalOffset;

    static void SetTileGeom();
    TileMesh(glm::vec2 localOffset, int edgeMorph = EdgeMorph::NONE);
    glm::vec2 GetLocalOffset();
    int GetEdgeMorph();
};