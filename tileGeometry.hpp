#pragma once

#include <vector>

#include <GL/glew.h>

using namespace std;


class TileGeometry {
private:
    static TileGeometry* instance;
    vector<GLfloat> vertices;
    vector<GLuint> indices;
    GLuint vaoId;
    GLuint vboId;
    GLuint iboId;

    void CreateTileVertices();
    void CreateTileIndices();
    void SetBuffers();
    void SetVBO();
    void SetIBO();
    TileGeometry();

public:
    const int tileSize = 64;

    ~TileGeometry();
    static TileGeometry* GetInstance();
    int GetIndicesSize();
    GLuint GetVaoId();
    GLuint GetVboId();
    GLuint GetIboId();
    GLuint* GetIndicesBufferPtr();
    vector<GLuint>* GetIndicesVector();
};