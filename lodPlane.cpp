#include <iostream>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "lodPlane.hpp"
#include "tileGeometry.hpp"
#include "tileMaterial.hpp"
#include "glDebug.hpp"


LODPlane::LODPlane(int windowW, int windowH, Camera *_camera) 
: camera(_camera) {
    CalcLayersNumber();
    // TileMesh::globalOffset = glm::vec2(0, 0);
    CreateTiles();
}

void LODPlane::CalcLayersNumber() {
    int zeroLevel = 4 * TileGeometry::GetInstance()->tileSize;
    layers = 1;
    while(zeroLevel * 2 <= planeWidth) {
        zeroLevel *= 2;
        layers++;
    }
    layers++; // Zliczamy "fałszywą" warstwę w samym środku
    lodMeshWidth = zeroLevel;
}

void LODPlane::CreateTiles() {
    tiles.resize(layers);
    int tileSize = TileGeometry::GetInstance()->tileSize;
    // @Refactor: Temporary fix. Można usunąć wszystkie wystąpienia tileSize w obliczeniach
    //            local offsetu w konstruktorach TileMesh.
    tileSize = 1;
    tiles[0].push_back(TileMesh(glm::vec2(-tileSize, -tileSize), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(-tileSize, 0), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, -tileSize), EdgeMorph::NONE));
    tiles[0].push_back(TileMesh(glm::vec2(0, 0), EdgeMorph::NONE));

    for(int layer = 0; layer < layers; layer++) {
        // Left strip
        int scale = pow(2, layer);
        tiles[layer].push_back(
            TileMesh(
                glm::vec2(-tileSize * scale * 2, -tileSize * scale * 2), 
                EdgeMorph::TOP | EdgeMorph::LEFT
            )
        );
        tiles[layer].push_back(
            TileMesh(glm::vec2(-tileSize * scale * 2, -tileSize * scale), EdgeMorph::LEFT)
        );
        tiles[layer].push_back(
            TileMesh(glm::vec2(-tileSize * scale * 2, 0), EdgeMorph::LEFT)
        );
        tiles[layer].push_back(
            TileMesh(
                glm::vec2(-tileSize * scale * 2, tileSize * scale), 
                EdgeMorph::LEFT | EdgeMorph::BOTTOM
            )
        );

        // Middle-left strip
        tiles[layer].push_back(
            TileMesh(glm::vec2(-tileSize * scale, -tileSize * scale * 2), EdgeMorph::TOP)
        );
        tiles[layer].push_back(
            TileMesh(glm::vec2(-tileSize * scale, tileSize * scale), EdgeMorph::BOTTOM)
        );

        // Middle-right strip
        tiles[layer].push_back(TileMesh(glm::vec2(0, -tileSize * scale * 2),  EdgeMorph::TOP));
        tiles[layer].push_back(TileMesh(glm::vec2(0, tileSize * scale),  EdgeMorph::BOTTOM));

        // Right strip
        tiles[layer].push_back(
            TileMesh(
                glm::vec2(tileSize * scale, -tileSize * scale * 2), 
                EdgeMorph::TOP | EdgeMorph::RIGHT
            )
        );
        tiles[layer].push_back(
            TileMesh(
                glm::vec2(tileSize * scale, -tileSize * scale), 
                EdgeMorph::RIGHT
            )
        );
        tiles[layer].push_back(TileMesh(glm::vec2(tileSize * scale, 0), EdgeMorph::RIGHT));
        tiles[layer].push_back(
            TileMesh(
                glm::vec2(tileSize * scale, tileSize * scale), 
                EdgeMorph::RIGHT | EdgeMorph::BOTTOM
            )
        );
    }
}

void LODPlane::Draw() {
    glUseProgram(TileMaterial::shader->GetProgramId());
    glBindVertexArray(TileGeometry::GetInstance()->GetVaoId());
    glm::mat4 viewMat = camera->GetViewMatrix();
    glUniformMatrix4fv(TileMaterial::GetUnifViewMat(), 1, GL_FALSE, glm::value_ptr(viewMat));
    int indicesSize = TileGeometry::GetInstance()->GetIndicesSize();
    for(int i = 0; i < tiles.size(); i++) {
        glUniform1i(tiles[i][0].material.GetUnifLevel(), i);
        for(int j = 0; j < tiles[i].size(); j++) {
            glUniform2f(tiles[i][j].material.GetUnifLocOffset(), tiles[i][j].GetLocalOffset().x,
                        tiles[i][j].GetLocalOffset().y);
            GetErrors();
            glUniform1i(tiles[i][j].material.GetUnifEdgeMorph(), tiles[i][j].GetEdgeMorph());
            GetErrors();
            glDrawElements(GL_TRIANGLE_STRIP, indicesSize, GL_UNSIGNED_INT, 
                           TileGeometry::GetInstance()->GetIndicesBufferPtr());
            GetErrors();
        }
    }
    glBindVertexArray(0);
}
