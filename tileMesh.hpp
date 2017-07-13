#pragma once

#include <glm/glm.hpp>

#include "tileGeometry.hpp"
#include "tileMaterial.hpp"
#include "edgeMorph.hpp"


class TileMesh {
private:
    static TileGeometry* tileGeom;
    static glm::vec2 globalOffset;
    glm::vec2 localOffset;
    int edgeMorph;
public:
    static void SetTileGeom();
    static void SetGlobalOffset(float x, float y);
    static glm::vec2 GetGlobalOffset();
    TileMesh(glm::vec2 localOffset, int edgeMorph = EdgeMorph::NONE);
    glm::vec2 GetLocalOffset() const;
    int GetEdgeMorph() const;
};