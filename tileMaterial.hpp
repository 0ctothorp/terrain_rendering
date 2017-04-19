#pragma once

#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "tileGeometry.hpp"

using namespace std;


class TileMaterial {
private:
    static const glm::mat4 projectionMatrix;
    static GLuint unifViewMat;
    static GLuint unifProjMat;
    GLuint unifLevel;
    GLuint unifLocOffset;
    GLuint unifGlobOffset;
    GLuint unifEdgeMorph;
public:
    static const string vertexShaderPath;
    static const string fragmentShaderPath;
    static Shader* shader;

    static GLuint GetUnifViewMat() { return unifViewMat; }
    static GLuint GetUnifProjMat() { return unifProjMat; }
    TileMaterial(glm::vec2 localOffset, int tileSize, int edgeMorph);
    GLuint GetUnifLevel() { return unifLevel; }
    GLuint GetUnifLocOffset() { return unifLocOffset; }
    GLuint GetUnifGlobOffset() { return unifGlobOffset; }
    GLuint GetUnifEdgeMorph() { return unifEdgeMorph; }
};