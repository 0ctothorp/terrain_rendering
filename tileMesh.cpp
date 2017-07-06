#include "tileMesh.hpp"


TileGeometry* TileMesh::tileGeom = nullptr;
glm::vec2 TileMesh::globalOffset{0, 0};

TileMesh::TileMesh(glm::vec2 localOffset, int edgeMorph)
: material(localOffset, TileGeometry::GetInstance()->tileSize, edgeMorph)
, localOffset(localOffset)
, edgeMorph(edgeMorph) {}

glm::vec2 TileMesh::GetLocalOffset() const { return localOffset; }

void TileMesh::SetTileGeom() {
    TileMesh::tileGeom = TileGeometry::GetInstance();
}

int TileMesh::GetEdgeMorph() const { return edgeMorph; }

void TileMesh::SetGlobalOffset(float x, float y) {
    globalOffset = glm::vec2(x, y);
}

glm::vec2 TileMesh::GetGlobalOffset() {
    return globalOffset;
}