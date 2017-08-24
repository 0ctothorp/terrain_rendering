#pragma once

#include <vector>

#include <GL/glew.h>

using namespace std;


// Singleton trzymający informacje na temat siatki pojedynczego kafelka terenu.
class TileGeometry {
private:
    static TileGeometry* instance;
    // Wektor współrzędnych wierzchołków.
    vector<GLfloat> vertices;
    // Wektor indeksów wierzchołków.
    vector<GLuint> indices;
    // Vertex array object, vertex buffer object i index buffer object.
    GLuint vaoId;
    GLuint vboId;
    GLuint iboId;

    // Tworzy wierzchołki kafelka.
    void CreateTileVertices();
    // Tworzy indeksy wierzchołków.
    void CreateTileIndices();
    // Tworzy i wypełnia danymi bufory wierzchołków i indeksów.
    void SetBuffers();
    // Tworzy i wypełnia vertex buffer object.
    void SetVBO();
    // Tworzy i wypełnia index buffer object
    void SetIBO();
    TileGeometry();

public:
    // Rozmiar kafelka wyrażony w ilości trójkątów mieszczących się na boku.
    static int tileSize;

    ~TileGeometry();
    static TileGeometry* GetInstance();
    // Zwraca rozmiar wektora 'indices'.
    int GetIndicesSize();
    GLuint GetVaoId();
    GLuint GetVboId();
    GLuint GetIboId();
    // Zwraca wskaźnik na miejsce w pamięci, gdzie znajduje się wektor indeksów.
    GLuint* GetIndicesBufferPtr();
};