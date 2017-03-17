#include <iostream>
#include <cassert>

#include <glm/gtc/type_ptr.hpp>

#include "plane.hpp"
#include "glDebug.hpp"

void Plane::CalcVerticesPositions(vector< vector<short> > *hmData) {
    int pos = 0;
    for(float i = 0.0f; i <= width; i++) {
        for(float j = 0.0f; j <= height; j++) {
            vertices[pos] = j;
            vertices[++pos] = (float)(*hmData)[i][j] / 200.0f;
            vertices[++pos] = i;
            pos++;
        }
    }
}

void Plane::CalcIndices() {
    int j = 0;
    indices[j] = 0;
    int i;
    for(i = 0; i < vertices.size() / 3 - (width + 1); i++) {
        if(i % (width + 1) == (uint)width) {
            indices[++j] = i + width + 1; 
            indices[++j] = i + width + 1; 
            indices[++j] = i + 1; 
            indices[++j] = i + 1;
        } else {
            indices[++j] = i + width + 1; 
            indices[++j] = i + 1;
        }
    }
    indices[++j] = i + width + 1;
}

void Plane::SetBuffers() {
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);       
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        &vertices[0],
        GL_STATIC_DRAW
    );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); 
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &ibo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_id);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLushort),
        &indices[0],
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // GetFirstNMessages(10);
}

Plane::Plane(int windowW, int windowH, int _width, int _height, Camera *_camera, 
             vector< vector<short> > *hmData) : 
width(_width), 
height(_height), 
shader(vertexShaderPath, fragmentShaderPath),
camera(_camera),
indices(4 * height + (2 * width * height + 2)),
vertices((width + 1) * (height + 1) * 3) {
    CalcVerticesPositions(hmData);
    CalcIndices();
    SetBuffers();
    glUseProgram(shader.GetProgramId());
    unifViewMat = glGetUniformLocation(shader.GetProgramId(), "viewMat");
    unifProjMat = glGetUniformLocation(shader.GetProgramId(), "projMat");
    projectionMatrix = glm::perspective(
        glm::radians(60.0f),
        (GLfloat)windowW / (GLfloat)windowH,
        0.01f,
        100.0f
    );
    glUniformMatrix4fv(unifProjMat, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // GetFirstNMessages(10);
}

Plane::~Plane() {
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(1, &ibo_id);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteProgram(shader_id);
}

void Plane::Draw() {
    glUseProgram(shader.GetProgramId());
    glBindVertexArray(vao_id);
    glm::mat4 viewMat = camera->GetViewMatrix();
    glUniformMatrix4fv(unifViewMat, 1, GL_FALSE, glm::value_ptr(viewMat));
    glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, &indices[0]);
    glBindVertexArray(0);
    // GetFirstNMessages(10);
}
