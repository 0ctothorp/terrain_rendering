#version 330 core

in float morphFactor;
flat in int sample_;
in vec3 poss;

out vec4 color;

uniform int level;
uniform isampler2D heightmap;
uniform sampler2D testTex;
uniform bool debug = true;
uniform int meshSize;

vec4 red = vec4(1, 0, 0, 1);
vec4 green = vec4(0, 1, 0, 1);
vec4 blue = vec4(0, 0, 1, 1);
vec4 yellow = green + red;

void main() {
    if(debug) {
        if(level % 4 == 0) color = red;
        else if(level % 4 == 1) color = green;
        else if(level % 4 == 2) color = blue;
        else color = yellow;
        if(morphFactor > 0)
            color = mix(color, vec4(1, 1, 1, 1), morphFactor);
        if(sample_ < -32000.0) color = vec4(0, 0, 0, 1);
        // color = mix(color, texture(testTex, poss.xz / meshSize + vec2(0.5, 0.5)), 0.5);
        // 1249 to max height
        color = vec4(sample_ / 1249.0f, .1, .1, 1);
    } else {
        color = mix(vec4(0, 1, 0, 1), vec4(1, 1, 1, 1), abs(sample_) / 8000.0);
    }
}