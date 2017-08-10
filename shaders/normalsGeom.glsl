#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 2) out;

in vec3 vertexNormal[];
in vec3 fragPos[];

uniform mat4 projMat;
uniform mat4 viewMat;

void main() {
    vec3 pos = fragPos[0];

    gl_Position = projMat * viewMat * vec4(pos, 1.0f);
    EmitVertex();

    gl_Position = projMat * viewMat * vec4(pos + vertexNormal[0] * 2.0f, 1.0f);
    EmitVertex();
    
    EndPrimitive();
}  