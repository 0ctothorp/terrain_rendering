#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "camera.hpp"

using namespace std;

class Plane {
private:
    int width;
    int height;
    vector<GLfloat> vertices;
    vector<GLuint> indices;
    GLuint vao_id;
    GLuint vbo_id;
    GLuint ibo_id;
    GLuint shader_id;
    const string vertexShaderPath = "shaders/planeVertexShader.glsl";
    const string fragmentShaderPath = "shaders/planeFragmentShader.glsl";
    Shader shader;
    glm::mat4 projectionMatrix;
    Camera *camera;
    GLuint unifViewMat;
    GLuint unifProjMat;

    // debug
    uint _indices = 0;

    void CalcVerticesPositions(vector< vector<short> > *);
    void CalcIndices();
    void SetBuffers();

public:
    // @Refactor: Loose coupling with camera instead tight.
    Plane(int windowW, int windowH, int _width = 5, int _height = 5, Camera *camera = nullptr,
          vector< vector<short> > *hmData = nullptr);
    ~Plane();
    void Draw();
};