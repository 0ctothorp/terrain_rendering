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
    static GLuint unifGlobOffset;
    GLuint unifLevel;
    GLuint unifLocOffset;
    GLuint unifEdgeMorph;

public:
    static const string vertexShaderPath;
    static const string fragmentShaderPath;
    static Shader* shader;

    TileMaterial(glm::vec2 localOffset, int tileSize, int edgeMorph);

    static void SetStaticUniforms();
    static GLuint GetUnifViewMat() { return unifViewMat; }
    static GLuint GetUnifProjMat() { return unifProjMat; }
    static GLuint GetUnifLoc(const string var);
    static GLuint GetUnifGlobOffset() { return unifGlobOffset; }
    GLuint GetUnifLevel()      { return unifLevel; }
    GLuint GetUnifLocOffset()  { return unifLocOffset; }
    GLuint GetUnifEdgeMorph()  { return unifEdgeMorph; }
};
