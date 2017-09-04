#pragma once


enum Light_ {Light_PrecalcNormals, Light_LivecalcNormals, Light_None};

struct TerrainRenderingSettings {
    bool wireframeMode = false;
    bool prevWireframeMode = wireframeMode;

    bool meshMovementLocked = false;
    bool prevMeshMovementLocked = meshMovementLocked;

    bool drawTerrainNormals = false;
    bool prevDrawTerrainNormals = drawTerrainNormals;

    bool drawTopView = false;
    bool prevDrawTopView = drawTopView;

    bool terrainVertexSnapping = true;
    bool prevTerrainVertexSnapping = terrainVertexSnapping;

    bool debugColors = false;
    bool prevDebugColors = debugColors;

    float lightPos[3] {0, 1000, 0};

    Light_ lightingType = Light_PrecalcNormals;
    Light_ prevLightingType = lightingType;
};
