#include "tileGeometry.hpp"


TileGeometry* TileGeometry::instance = nullptr;

TileGeometry* TileGeometry::GetInstance() {
    if(instance) return instance;
    instance = new TileGeometry();
    return instance;
}

void TileGeometry::CreateTileVertices() {
    vertices.resize((tileSize + 1) * (tileSize + 1) * 3);
    int pos = 0;
    for(int i = 0; i <= tileSize; i++) {
        for(int j = 0; j <= tileSize; j++) {
            vertices[pos] = (float)j / tileSize;
            vertices[++pos] = 0;
            vertices[++pos] = (float)i / tileSize;
            pos++;
        }
    }
}

void TileGeometry::CreateTileIndices() {
    TileGeometry::indices.resize(4 * (tileSize - 1) + 2 * tileSize * tileSize + 2);
    int j = 0;
    TileGeometry::indices[j] = 0;
    unsigned int i;
    for(i = 0; i < vertices.size() / 3 - (tileSize + 1) - 1; i++) {
        if(i % (tileSize + 1) == (unsigned int)tileSize) {
            TileGeometry::indices[++j] = i + tileSize + 1;
            TileGeometry::indices[++j] = i + tileSize + 1;
            TileGeometry::indices[++j] = i + 1;
            TileGeometry::indices[++j] = i + 1;
        } else {
            TileGeometry::indices[++j] = i + tileSize + 1;
            TileGeometry::indices[++j] = i + 1;
        }
    }
    TileGeometry::indices[++j] = i + tileSize + 1;
}

void TileGeometry::SetBuffers() {
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    SetVBO();
    SetIBO();

    // glBindVertexArray(0);
}

void TileGeometry::SetVBO() {
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);       
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        &vertices[0],
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); 
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TileGeometry::SetIBO() {
    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        &indices[0],
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

TileGeometry::TileGeometry() {
    if(instance != nullptr) throw "Singleton class TileGeometry already instantiated.";
    CreateTileVertices();
    CreateTileIndices();
    SetBuffers();
}

TileGeometry::~TileGeometry() {
    glDeleteBuffers(1, &vboId);
    glDeleteBuffers(1, &iboId);
    glDeleteVertexArrays(1, &vaoId);
}

int TileGeometry::GetIndicesSize() { return indices.size(); }

GLuint TileGeometry::GetVaoId() { return vaoId; }

GLuint TileGeometry::GetVboId() { return vboId; }

GLuint TileGeometry::GetIboId() { return iboId; }

GLuint* TileGeometry::GetIndicesBufferPtr() { return &(indices[0]); }

vector<GLuint>* TileGeometry::GetIndicesVector() { return &indices; }
