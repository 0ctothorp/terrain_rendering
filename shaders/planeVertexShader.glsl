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

const int LIGHT_PRECALC_NORMALS = 0;
const int LIGHT_LIVECALC_NORMALS = 1;
const int LIGHT_NONE = 2;

out float morphFactor;
out vec3 fragPos;
out vec3 vertexNormal;
out vec2 uv;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float morphRegion;
uniform int edgeMorph;
uniform vec2 localOffset;
uniform vec2 globalOffset;
uniform int level = 0;
uniform int tileSize;
uniform isampler2D heightmap;
uniform sampler2D normalMap;
uniform int meshSize = 1024;
uniform vec2 heightmapOffset;
uniform bool vertexSnapping = false;
uniform int lightType = LIGHT_NONE;
uniform float highestPoint;

vec3 localOffsetV3;
vec3 globalOffsetV3;
int scale;
int heightmapSize;
vec3 position;

/* Zwraca wartość z zakresu [0, 1] określającą w jakim stopniu przekształcania do kolejnego poziomu
   znajduje się 'pos'. */
float getMorphFactor(vec3 pos) {
    if(edgeMorph == EDGE_MORPH_BOTTOM && pos.z >= 1.0 - morphRegion)
        return (pos.z - 1.0f) / morphRegion + 1.0f;
    if(edgeMorph == EDGE_MORPH_TOP && pos.z <= morphRegion)
        return 1.0f - pos.z / morphRegion;
    if(edgeMorph == EDGE_MORPH_RIGHT && pos.x >= 1.0f - morphRegion)
        return (pos.x - 1.0f) / morphRegion + 1.0f;
    if(edgeMorph == EDGE_MORPH_LEFT && pos.x <= morphRegion)
        return 1.0f - pos.x / morphRegion;

    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_RIGHT))
        return max(max(0.0f, 1.0f - pos.z / morphRegion), (pos.x - 1.0f) / morphRegion + 1.0f);
    if(edgeMorph == (EDGE_MORPH_TOP | EDGE_MORPH_LEFT))
        return max(max(0.0f, 1.0f - pos.x / morphRegion), 1 - pos.z / morphRegion);
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_LEFT))
        return max(max(0.0f, 1.0f - pos.x / morphRegion), (pos.z - 1.0f) / morphRegion + 1);
    if(edgeMorph == (EDGE_MORPH_BOTTOM | EDGE_MORPH_RIGHT))
        return max(max(0.0f, (pos.x - 1.0f) / morphRegion + 1.0f), 
            (pos.z - 1.0f) / morphRegion + 1.0f);
    return 0.0f;
}

/* Oblicza wysokość dla 'pos' (sąsiedniego wierzchołka) odczytując wartość z heightmapy 
   i zwraca nowy wektor zawierający dane z 'pos' i obliczoną wysokość. */
vec3 calcHeightForNeighbourVertexPosition(vec3 pos) {
    // vec3 result = tileSize * (scale * pos + localOffsetV3) + globalOffsetV3;
    pos.y = texture(
        heightmap, 
        (pos.xz + vec2(meshSize / 2.0f, meshSize / 2.0f) 
            + (heightmapSize - meshSize) / 2.0f) / heightmapSize
    ).r / 40.0f;
    return pos;
}

/* Oblicza pozycję sąsiedniego wierzchołka siatki. */
vec3 getNeighbourVertexPosition(const int direction) {
    float offset = /* 1.0f / float(tileSize); */ /* mix(scale, scale * 2, morphFactor); */ 1.0f;
    switch(direction) {
    case DIR_UP:
        vec3 posUp = position - vec3(0, 0, scale);
        return calcHeightForNeighbourVertexPosition(posUp);
    case DIR_DOWN:
        vec3 posDown = position + vec3(0, 0, scale);
        return calcHeightForNeighbourVertexPosition(posDown);
    case DIR_LEFT:
        vec3 posLeft = position - vec3(scale, 0, 0);
        return calcHeightForNeighbourVertexPosition(posLeft);
    case DIR_RIGHT:
        vec3 posRight = position + vec3(scale, 0, 0);
        return calcHeightForNeighbourVertexPosition(posRight);
    case DIR_RIGHTUP:
        vec3 posUpRight = position + vec3(scale, 0, -scale);
        return calcHeightForNeighbourVertexPosition(posUpRight);
    case DIR_RIGHTDOWN:
        vec3 posDownRight = position + vec3(scale, 0, scale);
        return calcHeightForNeighbourVertexPosition(posDownRight);
    case DIR_LEFTUP:
        vec3 posUpLeft = position - vec3(scale, 0, scale);
        return calcHeightForNeighbourVertexPosition(posUpLeft);
    case DIR_LEFTDOWN:
        vec3 posDownLeft = position + vec3(-scale, 0, scale);
        return calcHeightForNeighbourVertexPosition(posDownLeft);
    default:
        return vec3(0, 0, 0);
    }
}

// Zwraca wektor normalny powierzchni trójkąta opisywanego przez 'v1', 'v2' i 'v3'.
vec3 getTriangleNormal(vec3 v1, vec3 v2, vec3 v3) {
    vec3 edge1 = v2 - v3;
    vec3 edge2 = v1 - v3;
    return normalize(cross(edge1, edge2));
}

/* Zwraca wektor normalny dla trójkąta zawierającego 'position', a którego kolejne 2 wierzchołki
   znajdują się w kierunkach 'direction1' i 'direction2'. Nie znamy ich konkretnych pozycji
   dlatego posługujemy się kierunkami. */
vec3 getNeighbourTriangleNormal(const int direction1, const int direction2, vec3 position) {
    vec3 vertex1 = getNeighbourVertexPosition(direction1);
    vec3 vertex2 = getNeighbourVertexPosition(direction2);
    return getTriangleNormal(vertex1, vertex2, position);
}

/* Zwraca wektor normalny dla wierzchołka 'position' wyliczając wektory normalne
   dla trójkątów zawierających ten wierzchołek, a następnie dodając je i normalizując
   sumę. */
vec3 getVertexNormal(vec3 position) {
    vec3 upperNormal = getNeighbourTriangleNormal(DIR_UP, DIR_RIGHTUP, position);
    vec3 upperRightNormal = getNeighbourTriangleNormal(DIR_RIGHTUP, DIR_RIGHT, position);
    vec3 lowerRightNormal = getNeighbourTriangleNormal(DIR_RIGHT, DIR_DOWN, position);
    vec3 lowerNormal = getNeighbourTriangleNormal(DIR_DOWN, DIR_LEFTDOWN, position);
    vec3 lowerLeftNormal = getNeighbourTriangleNormal(DIR_LEFTDOWN, DIR_LEFT, position);
    vec3 upperLeftNormal = getNeighbourTriangleNormal(DIR_LEFT, DIR_UP, position);
    return normalize(upperNormal + upperRightNormal + lowerRightNormal + lowerNormal + 
        lowerLeftNormal + upperLeftNormal);
}

void main() {
    localOffsetV3 = vec3(localOffset.x, 0, localOffset.y);
    globalOffsetV3 = vec3(globalOffset.x, 0, globalOffset.y);

    scale = int(pow(2, level));
    position = tileSize * (scale * pos + localOffsetV3);
    if(vertexSnapping) {
        position = floor((position + globalOffsetV3) / float(scale)) * float(scale);
    }
    morphFactor = getMorphFactor(pos);

    int scale2 = scale * 2;
    vec3 pos2 = floor(position / float(scale2)) * float(scale2);
    position = mix(position, pos2, morphFactor);      
    if(!vertexSnapping)
        position = mix(position, pos2, morphFactor) + globalOffsetV3;           

    heightmapSize = textureSize(heightmap, 0).x;
    uv = (position.xz + vec2(meshSize / 2.0f, meshSize / 2.0f) + (heightmapSize - meshSize) 
        / 2.0f) / heightmapSize;
    position.y = texture(heightmap, uv).r / 92.5f;

    if(lightType == LIGHT_LIVECALC_NORMALS) {
        float right = getNeighbourVertexPosition(DIR_RIGHT).y;
        float left = getNeighbourVertexPosition(DIR_LEFT).y;
        float up = getNeighbourVertexPosition(DIR_UP).y;
        float down = getNeighbourVertexPosition(DIR_DOWN).y;
        vertexNormal = normalize(vec3(left - right, scale, down - up));    
    } else if(lightType == LIGHT_PRECALC_NORMALS) {
        // vertexNormal = normalize(texture(normalMap, uv).rgb);
    } else {
        vertexNormal = vec3(0, 0, 0);
    }
    
    gl_Position = projMat * viewMat * vec4(position, 1.0f);
    fragPos = position;
}
