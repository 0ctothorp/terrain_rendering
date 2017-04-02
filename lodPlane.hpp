#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "camera.hpp"

using namespace std;

class LODPlane {
private:
    const int planeWidth = 128;
    const int tileSize = 1;
    const int layerWidthInTriangles = tileSize * 4;
    const int topBottomStripWidth = tileSize * 2;
    const int topBottomStripHeight = tileSize;
    const int leftRightStripWidth = tileSize;
    const int leftRightStripHeight = tileSize * 4;
    const float morphAreaFraction = 1.0 / 4.0;
    int meshWidth;
    int layers;
    vector<GLfloat> vertices;
    vector< vector<GLuint> > indicesTiles;

    GLuint vao_id;
    GLuint vbo_id;
    vector<GLuint> ibos;
    GLuint shader_id;
    const string vertexShaderPath = "shaders/planeVertexShader.glsl";
    const string fragmentShaderPath = "shaders/planeFragmentShader.glsl";
    Shader shader;
    glm::mat4 projectionMatrix;
    Camera *camera;
    GLuint unifViewMat;
    GLuint unifProjMat;
    GLuint unifLevel;

    void CalcLayersNumber();
    void CalcVerticesPositions();
    void CalcIndices();
    void SetUpLevelZeroIndices(const int mid);
    void SetUpTopAndBottomStripsIndicesForLevel(int level, const int mid);
    void SetUpLeftAndRightStripsIndicesForLevel(int level, const int mid);
    void SetBuffers();
    void CalcIndicesForLevelPart(const int topLeft, const int levelPartWidth, 
                                 const int levelPartHeight, const int iboNum, const int level);
    int GetIndicesSizeForDimensions(int width, int height);

public:
    LODPlane(int windowW, int windowH, Camera *camera = nullptr);
    ~LODPlane();
    void Draw();
};