#version 330 core

layout (location = 0) in vec3 pos;

uniform mat4 projMat;
uniform mat4 viewMat;
uniform float morphFactor = 0;

void main() {   
    gl_Position = projMat * viewMat * vec4(pos, 1.0f);
    if(morphFactor > 0) {}
}
