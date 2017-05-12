#version 330 core

in float morphFactor;
in float sample_;
in vec3 poss;

out vec4 color;

uniform int level;
uniform sampler2D heightmap;
uniform bool debug = true;
uniform int meshSize;

vec4 red = vec4(1, 0, 0, 1);
vec4 green = vec4(0, 1, 0, 1);
vec4 blue = vec4(0, 0, 1, 1);
vec4 yellow = green + red;

void main() {
    if(level % 4 == 0) color = red;
    else if(level % 4 == 1) color = green;
    else if(level % 4 == 2) color = blue;
    else color = yellow;
    if(morphFactor > 0) color = mix(color, vec4(1, 1, 1, 1), morphFactor);
    if(sample_ < -32000.0) color = vec4(0, 0, 0, 1);
    if(debug) color = mix(color, vec4(sample_, sample_, 0, 1), 0.7);
    else color = vec4(sample_, .1, .1, 1);
}