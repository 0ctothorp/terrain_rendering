#pragma once

#include <glm/glm.hpp>

#include "edgeMorph.hpp"


// Przechowuje informacje na temat siatki kafelka.
class TileMesh {
private:
    glm::vec2 localOffset;
    int edgeMorph;
    
public:
    TileMesh(glm::vec2 localOffset, int edgeMorph = EdgeMorph::NONE);
    glm::vec2 GetLocalOffset() const;
    int GetEdgeMorph() const;
};