#version 330 core

in float morphFactor;
in float sample_;
in vec3 fragPos;
in vec3 vertexNormal;

out vec4 fragColor;

uniform int level;
uniform sampler2D heightmap;
uniform bool debug = true;
uniform int meshSize;
uniform vec3 lightPosition;

vec4 red = vec4(1, 0, 0, 1);
vec4 green = vec4(0, 1, 0, 1);
vec4 blue = vec4(0, 0, 1, 1);
vec4 yellow = green + red;

void main() {
    vec4 color;
    if(level % 4 == 0) color = red;
    else if(level % 4 == 1) color = green;
    else if(level % 4 == 2) color = blue;
    else color = yellow;
    if(morphFactor > 0) color = mix(color, vec4(1, 1, 1, 1), morphFactor);
    float _sample = sample_ * 15;
    // if(_sample < -32000.0) color = vec4(0, 0, 0, 1);
    if(debug) color = mix(color, vec4(_sample, _sample, 0, 1), 0.7);
    else color = vec4(_sample, .1, .1, 1);

    // fragColor = color;

    vec3 lightDirection = normalize(lightPosition - fragPos);

    float diffuseImpact = max(dot(vertexNormal, lightDirection), 0.0f);
    vec3 lightColor = vec3(1, 1, 1);
    vec3 diffuseComponent = diffuseImpact * lightColor;

    float ambientStrength = 0.5f;
    vec3 ambient = ambientStrength * lightColor;
    fragColor = vec4((ambient + diffuseComponent) * vec3(color.x, color.y, color.z), 1.0f);
}