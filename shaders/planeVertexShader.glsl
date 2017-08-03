#version 330 core

layout (location = 0) in vec3 pos;

const int EDGE_MORPH_TOP = 1;
const int EDGE_MORPH_BOTTOM = 2;
const int EDGE_MORPH_LEFT = 4;
const int EDGE_MORPH_RIGHT = 8;
const int EDGE_MORPH_NONE = 0;

const int DIR_UP = 0;
const int DIR_DOWN = 1;
const int DIR_RIGHTUP = 2;
const int DIR_RIGHTDOWN = 3;
const int DIR_LEFTUP = 4;
const int DIR_LEFTDOWN = 5;
const int DIR_RIGHT = 6;
const int DIR_LEFT = 7;

out float morphFactor;
out float sample_;
out vec3 fragPos;
out vec3 vertexNormal;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float morphRegion;
uniform int edgeMorph;
uniform vec2 localOffset;
uniform vec2 globalOffset;
uniform int level = 0;
uniform int tileSize;
uniform sampler2D heightmap;
uniform int meshSize = 1024;
uniform vec2 heightmapOffset;

vec3 localOffsetV3;
vec3 globalOffsetV3;
int scale;
int heightmapSize;

float getMorphFactor(vec3 pos) {
    if(edgeMorph == EDGE_MORPH_BOTTOM && pos.z >= 1.0 - morphRegion)
        return (pos.z - 1.0) / morphRegion + 1;
    if(edgeMorph == EDGE_MORPH_TOP && pos.z <= morphRegion)
        return 1 - pos.z / morphRegion;
    if(edgeMorph == EDGE_MORPH_RIGHT && pos.x >= 1.0 - morphRegion)
        return (pos.x - 1.0) / morphRegion + 1;
    if(edgeMorph == EDGE_MORPH_LEFT && pos.x <= morphRegion)
        return 1 - pos.x / morphRegion;

    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_RIGHT))
        return max(max(0.0, 1 - pos.z / morphRegion), (pos.x - 1.0) / morphRegion + 1);
    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_LEFT))
        return max(max(0.0, 1 - pos.x / morphRegion), 1 - pos.z / morphRegion);
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_LEFT))
        return max(max(0.0, 1 - pos.x / morphRegion), (pos.z - 1.0) / morphRegion + 1);
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_RIGHT))
        return max(max(0.0, (pos.x - 1.0) / morphRegion + 1), (pos.z - 1.0) / morphRegion + 1);
    return 0.0;
}

vec3 calcArbitraryVertexPosition(vec3 pos) {
    vec3 result = tileSize * (scale * pos + localOffsetV3) + globalOffsetV3;
    result.y = texture(
        heightmap, 
        (result.xz + vec2(meshSize / 2.0f, meshSize / 2.0f) 
            + (heightmapSize - meshSize) / 2.0f) / heightmapSize
    ).r * 750;
    return result;
}

vec3 getArbitraryVertexPosition(const int direction) {
    float offset = 1.0f / float(tileSize);
    switch(direction) {
    case DIR_UP:
        vec3 posUp = pos - vec3(0, 0, offset);
        return calcArbitraryVertexPosition(posUp);
    case DIR_DOWN:
        vec3 posDown = pos + vec3(0, 0, offset);
        return calcArbitraryVertexPosition(posDown);
    case DIR_LEFT:
        vec3 posLeft = pos - vec3(offset, 0, 0);
        return calcArbitraryVertexPosition(posLeft);
    case DIR_RIGHT:
        vec3 posRight = pos + vec3(offset, 0, 0);
        return calcArbitraryVertexPosition(posRight);
    case DIR_RIGHTUP:
        vec3 posUpRight = pos + vec3(offset, 0, -offset);
        return calcArbitraryVertexPosition(posUpRight);
    case DIR_RIGHTDOWN:
        vec3 posDownRight = pos + vec3(offset, 0, offset);
        return calcArbitraryVertexPosition(posDownRight);
    case DIR_LEFTUP:
        vec3 posUpLeft = pos - vec3(offset, 0, offset);
        return calcArbitraryVertexPosition(posUpLeft);
    case DIR_LEFTDOWN:
        vec3 posDownLeft = pos + vec3(-offset, 0, offset);
        return calcArbitraryVertexPosition(posDownLeft);
    default:
        return vec3(0, 0, 0);
    }
}

vec3 getTriangleNormal(vec3 v1, vec3 v2, vec3 v3) {
    vec3 edge1 = v1 - v2;
    vec3 edge2 = v2 - v3;
    return cross(edge1, edge2);
}

vec3 getArbitraryTriangleNormal(const int direction1, const int direction2, vec3 position) {
    vec3 vertex1 = getArbitraryVertexPosition(direction1);
    vec3 vertex2 = getArbitraryVertexPosition(direction2);
    return getTriangleNormal(vertex1, vertex2, position);
}

vec3 getVertexNormal(vec3 position) {
    vec3 upperNormal = getArbitraryTriangleNormal(DIR_UP, DIR_RIGHTUP, position);
    vec3 upperRightNormal = getArbitraryTriangleNormal(DIR_RIGHTUP, DIR_RIGHT, position);
    vec3 lowerRightNormal = getArbitraryTriangleNormal(DIR_RIGHT, DIR_DOWN, position);
    vec3 lowerNormal = getArbitraryTriangleNormal(DIR_DOWN, DIR_LEFTDOWN, position);
    vec3 lowerLeftNormal = getArbitraryTriangleNormal(DIR_LEFTDOWN, DIR_LEFT, position);
    vec3 upperLeftNormal = getArbitraryTriangleNormal(DIR_LEFT, DIR_UP, position);
    return normalize(upperNormal + upperRightNormal + lowerRightNormal + lowerNormal + 
        lowerLeftNormal + upperLeftNormal);
}

void main() {
    localOffsetV3 = vec3(localOffset.x, 0, localOffset.y);
    globalOffsetV3 = vec3(globalOffset.x, 0, globalOffset.y);

    scale = int(pow(2, level));
    vec3 position = tileSize * (scale * pos + vec3(localOffset.x, 0, localOffset.y)) 
        + vec3(globalOffset.x, 0, globalOffset.y);
    morphFactor = getMorphFactor(pos);
    position = floor(position / scale) * scale; // snapping
    // powoduje, że zamiast siatka płynnie przechodzić na kolejne
    // teksele(?) i uprawiać drżenie, skacze co 'scale' wierzchołków.

    if(morphFactor > 0.0) {
        int scale2 = scale * 2;
        vec3 pos2 = floor(position / scale2) * scale2;
        position = mix(position, pos2, morphFactor);        
    }
    // position += vec3(globalOffset.x, 0, globalOffset.y);
    heightmapSize = textureSize(heightmap, 0).x;
    sample_ = texture(
        heightmap, 
        (position.xz + vec2(meshSize / 2.0f, meshSize / 2.0f) 
            + (heightmapSize - meshSize) / 2.0f) / heightmapSize
    ).r;
    
    position.y = sample_ * 750 /*363*/;
    
    vertexNormal = getVertexNormal(position);

    gl_Position = projMat * viewMat * vec4(position, 1.0f);
    fragPos = position;
}
