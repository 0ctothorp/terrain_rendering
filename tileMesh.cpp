#include "tileMesh.hpp"


TileMesh::TileMesh(glm::vec2 localOffset, int edgeMorph)
: localOffset(localOffset)
, edgeMorph(edgeMorph) {}

glm::vec2 TileMesh::GetLocalOffset() const { return localOffset; }

int TileMesh::GetEdgeMorph() const { return edgeMorph; }
