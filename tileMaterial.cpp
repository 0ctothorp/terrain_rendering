#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tileMaterial.hpp"


const string TileMaterial::vertexShaderPath = "shaders/planeVertexShader.glsl";
const string TileMaterial::fragmentShaderPath = "shaders/planeFragmentShader.glsl";
const glm::mat4 TileMaterial::projectionMatrix = glm::perspective(
    glm::radians(60.0f), 
    // @Refactor: zastąpić te wartości zmiennymi
    800.0f / 600.0f,
    0.01f,
    1000.0f
);
Shader* TileMaterial::shader = nullptr;
GLuint TileMaterial::unifViewMat{};
GLuint TileMaterial::unifProjMat{};


TileMaterial::TileMaterial(glm::vec2 localOffset, int tileSize, int edgeMorph) {
    glUseProgram(shader->GetProgramId());
    unifViewMat = glGetUniformLocation(shader->GetProgramId(), "viewMat");
    unifProjMat = glGetUniformLocation(shader->GetProgramId(), "projMat");
    glUniformMatrix4fv(unifProjMat, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    unifLevel = glGetUniformLocation(shader->GetProgramId(), "level");
    unifLocOffset = glGetUniformLocation(shader->GetProgramId(), "localOffset");
    unifGlobOffset = glGetUniformLocation(shader->GetProgramId(), "globalOffset");

    GLuint unifTileSize = glGetUniformLocation(shader->GetProgramId(), "tileSize");
    glUniform1i(unifTileSize, TileGeometry::GetInstance()->tileSize);

    unifEdgeMorph = glGetUniformLocation(shader->GetProgramId(), "edgeMorph");
    glUniform1f(glGetUniformLocation(shader->GetProgramId(), "morphRegion"), 0.3f);
}