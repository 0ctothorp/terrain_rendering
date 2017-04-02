#version 330 core

out vec4 color;

uniform int level;

vec4 red = vec4(1, 0, 0, 1);
vec4 green = vec4(0, 1, 0, 1);
vec4 blue = vec4(0, 0, 1, 1);
vec4 yellow = green + red;

void main() {
    if(level % 4 == 0) color = red;
    else if(level % 4 == 1) color = green;
    else if(level % 4 == 2) color = blue;
    else color = yellow;
}