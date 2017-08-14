#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() { 
    vec3 a = (texture(screenTexture, TexCoords).rgb + vec3(1, 1, 1)) / 2.0f;
    FragColor = vec4(a, 1.0f);
}