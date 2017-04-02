#include <iostream>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

#include "lodPlane.hpp"

LODPlane::LODPlane(int windowW, int windowH, Camera *_camera) 
: vertices((planeWidth + 1) * (planeWidth + 1) * 3)
, camera(_camera)
, shader(vertexShaderPath, fragmentShaderPath) {
    CalcLayersNumber();
    CalcVerticesPositions();
    CalcIndices();

    SetBuffers();
    glUseProgram(shader.GetProgramId());
    unifViewMat = glGetUniformLocation(shader.GetProgramId(), "viewMat");
    unifProjMat = glGetUniformLocation(shader.GetProgramId(), "projMat");
    unifLevel = glGetUniformLocation(shader.GetProgramId(), "level");
    projectionMatrix = glm::perspective(
        glm::radians(60.0f),
        (GLfloat)windowW / (GLfloat)windowH,
        0.01f,
        1000.0f
    );
    glUniformMatrix4fv(unifProjMat, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
}

void LODPlane::CalcLayersNumber() {
    int zeroLevel = 4 * tileSize;
    layers = 1;
    while(zeroLevel * 2 <= planeWidth) {
        zeroLevel *= 2;
        layers++;
    }
    meshWidth = zeroLevel;

    cout << meshWidth << " <- meshWidth, " << layers << " <- layers\n";
}

void LODPlane::CalcVerticesPositions() {
    int pos = 0;
    for(int i = 0; i <= planeWidth; i++) {
        for(int j = 0; j <= planeWidth; j++) {
            vertices[pos] = j;
            vertices[++pos] = 0;
            vertices[++pos] = i;
            pos++;
        }
    }
}

/*
    @Description: Gets indices buffer size for given dimensions.
    @Param width: width in vertices number.
    @Param height: height in vertices number.
*/
int LODPlane::GetIndicesSizeForDimensions(int width, int height) {
    return 4 * (height - 2) + 2 * (width - 1) * (height - 1) + 2;
}

void LODPlane::CalcIndicesForLevelPart(const int topLeft, const int levelPartWidth, 
                                       const int levelPartHeight, const int iboNum,
                                       const int level) {
    const int skip = pow(2, level);
    int j = 0;
    indicesTiles[iboNum][j] = topLeft;
    int currPos = topLeft;
    for(int i = 0; i < (levelPartWidth + 1) * (levelPartHeight + 1) - levelPartWidth - 2; i++) {
        if(i % (levelPartWidth + 1) == levelPartWidth) {
            int degenerate = currPos + (meshWidth + 1) * skip;
            indicesTiles[iboNum][++j] = degenerate;
            indicesTiles[iboNum][++j] = degenerate;
            currPos = degenerate - levelPartWidth * skip; // bo teraz levelPartWidth i Height oznaczają szerokość i wysokość co skip wierchołków
            indicesTiles[iboNum][++j] = currPos;
            indicesTiles[iboNum][++j] = currPos;
        } else {
            indicesTiles[iboNum][++j] = currPos + (meshWidth + 1) * skip; 
            currPos += skip;
            indicesTiles[iboNum][++j] = currPos;
        }
    }
    indicesTiles[iboNum][++j] = currPos + (meshWidth + 1) * skip;
}

void LODPlane::SetUpLevelZeroIndices(const int mid) {
    const int levelWidth = tileSize * 4;
    const int topLeft = mid - 2 * tileSize - (levelWidth / 2) * (meshWidth + 1);
    indicesTiles[0].resize(GetIndicesSizeForDimensions(levelWidth + 1, levelWidth + 1));
    CalcIndicesForLevelPart(topLeft, levelWidth, levelWidth, 0, 0);
}

void LODPlane::SetUpTopAndBottomStripsIndicesForLevel(int level, const int mid) {
    const int levelPartWidth = pow(2, level) * tileSize * 2;
    const int levelPartHeight = levelPartWidth / 2;
    const int iboNum = (level - 1) * 4 + 1; 
    int topLeft = mid - levelPartWidth / 2 - (levelPartHeight * 2) * (meshWidth + 1);
    const int indicesSize = GetIndicesSizeForDimensions(topBottomStripWidth + 1, 
                                                        topBottomStripHeight + 1);
    indicesTiles[iboNum].resize(indicesSize);
    CalcIndicesForLevelPart(topLeft, topBottomStripWidth, topBottomStripHeight, iboNum, level);
    topLeft = mid - levelPartWidth / 2 + levelPartHeight * (meshWidth + 1);
    indicesTiles[iboNum + 1].resize(indicesSize);
    CalcIndicesForLevelPart(topLeft, topBottomStripWidth, topBottomStripHeight, iboNum + 1, level);
}

void LODPlane::SetUpLeftAndRightStripsIndicesForLevel(int level, const int mid) {
    const int levelPartWidth = pow(2, level) * tileSize;
    const int levelPartHeight = levelPartWidth * 4;
    const int iboNum = (level - 1) * 4 + 3; 
    int topLeft = mid - levelPartWidth * 2 - (levelPartHeight / 2) * (meshWidth + 1);
    const int indicesSize = GetIndicesSizeForDimensions(leftRightStripWidth + 1, 
                                                        leftRightStripHeight + 1);
    indicesTiles[iboNum].resize(indicesSize);
    CalcIndicesForLevelPart(topLeft, leftRightStripWidth, leftRightStripHeight, iboNum, level);
    topLeft = mid + levelPartWidth - (levelPartHeight / 2) * (meshWidth + 1);
    indicesTiles[iboNum + 1].resize(indicesSize);
    CalcIndicesForLevelPart(topLeft, leftRightStripWidth, leftRightStripHeight, iboNum + 1, level);
}

void LODPlane::CalcIndices() {
    // every layer consists of 4 tile strips (top, down, sides) instead of 12 square tiles
    // except layer number 0 (the finest) which is a 4x4 tiles square
    indicesTiles.resize(1 + (layers - 1) * 4);

    const int mid = (meshWidth / 2) * (meshWidth + 1) + meshWidth / 2;
    camera->Position.x = vertices[mid * 3];
    camera->Position.z = vertices[mid * 3 + 2];

    SetUpLevelZeroIndices(mid);
    for(int level = 1; level < layers; level++) {
        SetUpTopAndBottomStripsIndicesForLevel(level, mid);
        SetUpLeftAndRightStripsIndicesForLevel(level, mid);
    }
}

void LODPlane::SetBuffers() {
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ibos.resize((layers - 1) * 4 + 1);
    glGenBuffers(ibos.size(), &ibos[0]);
    for(int i = 0; i < ibos.size(); i++) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indicesTiles[i].size() * sizeof(GLuint),
            &(indicesTiles[i][0]),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);
}

LODPlane::~LODPlane() {
    glDeleteBuffers(1, &vbo_id);
    glDeleteBuffers(ibos.size(), &ibos[0]);
    glDeleteVertexArrays(1, &vao_id);
    glDeleteProgram(shader_id);
}

void LODPlane::Draw() {
    glUseProgram(shader.GetProgramId());
    glBindVertexArray(vao_id);
    glm::mat4 viewMat = camera->GetViewMatrix();
    glUniformMatrix4fv(unifViewMat, 1, GL_FALSE, glm::value_ptr(viewMat));
    int level = 0;
    for(int i = 0; i < ibos.size(); i++) {
        glUniform1i(unifLevel, level);
        glDrawElements(GL_TRIANGLE_STRIP, indicesTiles[i].size(), GL_UNSIGNED_INT, 
                       &(indicesTiles[i][0]));
        if(i % 4 == 0) level++;
    }
    glBindVertexArray(0);
}
