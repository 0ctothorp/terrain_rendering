#include "tileMesh.hpp"


glm::vec2 TileMesh::globalOffset{0, 0};

TileMesh::TileMesh(glm::vec2 localOffset, int edgeMorph)
: localOffset(localOffset)
, edgeMorph(edgeMorph) {}

glm::vec2 TileMesh::GetLocalOffset() const { return localOffset; }

int TileMesh::GetEdgeMorph() const { return edgeMorph; }

void TileMesh::SetGlobalOffset(float x, float y) {
    globalOffset = glm::vec2(x, y);
}

glm::vec2 TileMesh::GetGlobalOffset() {
    return globalOffset;
}
