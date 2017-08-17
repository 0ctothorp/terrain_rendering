#version 330 core

const int LIGHT_PRECALC_NORMALS = 0;
const int LIGHT_LIVECALC_NORMALS = 1;
const int LIGHT_NONE = 2;

const int MAX_H = 750;

in float morphFactor;
in vec3 fragPos;
in vec3 vertexNormal;
in vec2 uv;

out vec4 fragColor;

uniform int level;
uniform isampler2D heightmap;
uniform sampler1D terrainColors;
uniform bool debug = false;
uniform int meshSize;
uniform vec3 lightPosition;
uniform int lightType = LIGHT_NONE;
uniform float highestPoint;

vec4 red = vec4(1, 0, 0, 1);
vec4 green = vec4(0, 1, 0, 1);
vec4 blue = vec4(0, 0, 1, 1);
vec4 yellow = green + red;

void main() {
    vec4 color;
    if(debug) {
        if(level % 4 == 0) color = red;
        else if(level % 4 == 1) color = green;
        else if(level % 4 == 2) color = blue;
        else color = yellow;
        if(morphFactor > 0) color = mix(color, vec4(1, 1, 1, 1), morphFactor);
    } else {
        color = texture(terrainColors, fragPos.y / 35.0f);        
    }
    
    if(lightType != LIGHT_NONE) {
        vec3 lightDirection = normalize(lightPosition - fragPos);

        float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0f);
        vec3 lightColor = vec3(1, 1, 1);
        vec3 diffuseComponent = diffuseImpact * lightColor;

        float ambientStrength = 0.1f;
        vec3 ambient = ambientStrength * lightColor;
        fragColor = vec4((ambient + diffuseComponent) * vec3(color.x, color.y, color.z), 1.0f);
    } else {
        fragColor = color;
    }
}