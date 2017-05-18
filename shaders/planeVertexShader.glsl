#version 330 core

#define EDGE_MORPH_TOP 1
#define EDGE_MORPH_BOTTOM 2
#define EDGE_MORPH_LEFT 4
#define EDGE_MORPH_RIGHT 8
#define EDGE_MORPH_NONE 0

layout (location = 0) in vec3 pos;

out float morphFactor;
out float sample_;
out vec3 poss;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float morphRegion;
uniform int edgeMorph;
uniform vec2 localOffset = vec2(0, 0);
uniform vec2 globalOffset = vec2(0, 0);
uniform int level = 0;
uniform int tileSize;
uniform sampler2D heightmap;
uniform int meshSize = 1024;
uniform vec2 heightmapOffset;

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

void main() {
    int scale = int(pow(2, level));
    vec3 position = tileSize * (scale * pos + vec3(localOffset.x, 0, localOffset.y)) + vec3(globalOffset.x, 0, globalOffset.y);
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
    int heightmapSize = textureSize(heightmap, 0).x;
    sample_ = texture(
        heightmap, 
        (position.xz + vec2(512.0f, 512.0f) + (heightmapSize - meshSize) / 2.0f) / heightmapSize
    ).r;
    position.y = sample_ * 750;
    gl_Position = projMat * viewMat * vec4(position, 1.0f);
    poss = position;
}
