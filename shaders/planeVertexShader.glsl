#version 330 core

#define EDGE_MORPH_TOP 1
#define EDGE_MORPH_BOTTOM 2
#define EDGE_MORPH_LEFT 4
#define EDGE_MORPH_RIGHT 8
#define EDGE_MORPH_NONE 0

layout (location = 0) in vec3 pos;

out float morphFactor;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float morphRegion;
uniform int edgeMorph;
uniform vec2 localOffset = vec2(0, 0);
uniform vec2 globalOffset = vec2(0, 0);
uniform int level = 0;
uniform int tileSize;

float getMorphFactor(vec3 pos) {
    if(edgeMorph == EDGE_MORPH_BOTTOM && pos.z >= 1.0 - morphRegion)
        return (pos.z - 1.0) / morphRegion + 1;
    if(edgeMorph == EDGE_MORPH_TOP && pos.z <= morphRegion)
        return 1 - pos.z / morphRegion;
    if(edgeMorph == EDGE_MORPH_RIGHT && pos.x >= 1.0 - morphRegion)
        return (pos.x - 1.0) / morphRegion + 1;
    if(edgeMorph == EDGE_MORPH_LEFT && pos.x <= morphRegion)
        return 1 - pos.x / morphRegion;

    float _morphFactor = 0.0;
    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_RIGHT)) {
        _morphFactor = max(0.0, 1 - pos.z / morphRegion);
        return max(_morphFactor, (pos.x - 1.0) / morphRegion + 1);
    }
    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_LEFT)) {
        _morphFactor = max(0.0, 1 - pos.z / morphRegion);
        return max(_morphFactor, 1 - pos.x / morphRegion);
    }
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_LEFT)) {
        _morphFactor = max(_morphFactor, (pos.z - 1.0) / morphRegion + 1);
        return max(_morphFactor, 1 - pos.x / morphRegion);
    }
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_RIGHT)) {
        _morphFactor = max((pos.z - 1.0) / morphRegion + 1, _morphFactor);
        return max(_morphFactor, (pos.x - 1.0) / morphRegion + 1);
    }
    return 0.0;
}

void main() {
    int scale = int(pow(2, level));
    vec3 position = tileSize * scale * pos;
    morphFactor = getMorphFactor(pos);
    if(morphFactor > 0.0) {
        int scale2 = scale * 2;
        vec3 pos2 = floor(position / scale2) * scale2;
        position = mix(position, pos2, morphFactor);        
    }
    position += tileSize * vec3(localOffset.x, 0, localOffset.y) + vec3(globalOffset, 0);
    gl_Position = projMat * viewMat * vec4(position, 1.0f);
}
