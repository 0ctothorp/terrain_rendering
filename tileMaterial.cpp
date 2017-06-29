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

TileMaterial::TileMaterial(glm::vec2 localOffset, int tileSize, int edgeMorph) {
    if(shader == nullptr) throw "Static field shader not initialized.";
    shader->Use();
    unifLevel      = GL_CHECK(shader->GetUniformLocation("level"));
    unifLocOffset  = GL_CHECK(shader->GetUniformLocation("localOffset"));
    unifEdgeMorph  = GL_CHECK(shader->GetUniformLocation("edgeMorph"));

    GL_CHECK(glUniform1i(shader->GetUniformLocation("tileSize"), TileGeometry::GetInstance()->tileSize));
    GL_CHECK(glUniform1f(shader->GetUniformLocation("morphRegion"), LODPlane::morphRegion));
}

void TileMaterial::SetStaticUniforms() {
    if(shader == nullptr) throw "Static field shader not initialized.";
    shader->Use();
    unifViewMat = GL_CHECK(shader->GetUniformLocation("viewMat"));
    unifProjMat = GL_CHECK(shader->GetUniformLocation("projMat"));
    unifGlobOffset = GL_CHECK(shader->GetUniformLocation("globalOffset"));
    GL_CHECK(glUniformMatrix4fv(unifProjMat, 1, GL_FALSE, glm::value_ptr(projectionMatrix)));
    GL_CHECK(glUniform1i(shader->GetUniformLocation("meshSize"), LODPlane::planeWidth));
}
