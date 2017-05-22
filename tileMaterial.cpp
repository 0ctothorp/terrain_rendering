#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tileMaterial.hpp"
#include "glDebug.hpp"
#include "lodPlane.hpp"
#include "window.hpp"
#include "camera.hpp"


const string TileMaterial::vertexShaderPath = "shaders/planeVertexShader.glsl";
const string TileMaterial::fragmentShaderPath = "shaders/planeFragmentShader.glsl";
const glm::mat4 TileMaterial::projectionMatrix = glm::perspective(
    glm::radians(Camera::fov), 
    (float)Window::width / (float)Window::height,
    0.01f,
    2000.0f
);
Shader* TileMaterial::shader = nullptr;
GLuint TileMaterial::unifViewMat{};
GLuint TileMaterial::unifProjMat{};
GLuint TileMaterial::unifGlobOffset{};


GLuint TileMaterial::GetUnifLoc(const string var) {
    return glGetUniformLocation(shader->GetProgramId(), var.c_str());
}

TileMaterial::TileMaterial(glm::vec2 localOffset, int tileSize, int edgeMorph) {
    if(shader == nullptr) throw "Static field shader not initialized.";
    GL_CHECK(glUseProgram(shader->GetProgramId()));
    unifLevel      = GL_CHECK(GetUnifLoc("level"));
    unifLocOffset  = GL_CHECK(GetUnifLoc("localOffset"));
    unifEdgeMorph  = GL_CHECK(GetUnifLoc("edgeMorph"));

    GL_CHECK(glUniform1i(GetUnifLoc("tileSize"), TileGeometry::GetInstance()->tileSize));
    GL_CHECK(glUniform1f(GetUnifLoc("morphRegion"), LODPlane::morphRegion));
}

void TileMaterial::SetStaticUniforms() {
    if(shader == nullptr) throw "Static field shader not initialized.";
    GL_CHECK(glUseProgram(shader->GetProgramId()));
    unifViewMat = GL_CHECK(GetUnifLoc("viewMat"));
    unifProjMat = GL_CHECK(GetUnifLoc("projMat"));
    unifGlobOffset = GL_CHECK(GetUnifLoc("globalOffset"));
    GL_CHECK(glUniformMatrix4fv(unifProjMat, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));
    GL_CHECK(glUniform1i(GetUnifLoc("meshSize"), LODPlane::planeWidth));
}
