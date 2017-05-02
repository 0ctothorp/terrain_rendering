#include "tileMesh.hpp"


TileGeometry* TileMesh::tileGeom = nullptr;

TileMesh::TileMesh(glm::vec2 localOffset, int edgeMorph)
: material(localOffset, TileGeometry::GetInstance()->tileSize, edgeMorph)
, localOffset(localOffset)
, edgeMorph(edgeMorph) {}

glm::vec2 TileMesh::GetLocalOffset() { return localOffset; }

void TileMesh::SetTileGeom() {
    TileMesh::tileGeom = TileGeometry::GetInstance();
}

int TileMesh::GetEdgeMorph() { return edgeMorph; }