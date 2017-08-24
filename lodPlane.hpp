#pragma once

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mainCamera.hpp"
#include "tileMesh.hpp"
#include "shader.hpp"


/* Klasa służąca do utworzenia całej struktury LOD siatki terenu i do jej rysowania. */
class LODPlane {
private:
    // Identyfikator tekstury heightmapy.
    GLuint heightmapTex;
    // Identyfikator tekstury zawierającej normalne terenu.
    GLuint normalMapTex;
    // Identyfikator tekstury zawierającej kolory, którymi pokolorowana jest siatka terenu.
    GLuint terrainColorsTex;
    /* Kolekcja zawierająca informacje na temat poszczególnych kafelków składających się na 
       siatkę terenu. Pod indeksem [x][_] znajduje się kafelek na poziomie LOD równym x. */
    std::vector< std::vector<TileMesh> > tiles;
    // Liczba warstw (poziomów).
    int layers;
    /* Szerokość siatki wyrażona w liczbie trójkątów o rozmiarze takim, jak w kafelku o poziomie 
       zerowym - najgęstszym. */
    int planeWidth;
    // Odpowiada na pytanie, czy siatka terenu ma się przesuwać razem z kamerą.
    bool meshMovementLocked = false;
    // Odpowiada na pytanie, czy rysować wektory normalne dla wierzchołków terenu.
    bool debugNormals = false;

    // Oblicza liczbę poziomów LOD i zapisuje ją do zmiennej 'layers'.
    void CalcLayersNumber();
    // Wypełnia kolekcję 'tiles' danymi na temat tworzonych kafelków.
    void CreateTiles();
    /* Odpowiada na pytanie, czy kafelek znajdujący się w 'tiles' pod indeksami [i][j] znajduje
       się w widoku kamery 'mainCam'. */
    bool IsTileInsideFrustum(int i, int j, const MainCamera &mainCam) const;
    /* Tworzy obiekty OpenGL dla tekstur heightmapy i normalnych terenu oraz 
       odpowiednio je konfiguruje. 'heightmapsPaths' zawiera ścieżki do map wysokości,
       które mają zostac połączone i stworzona z nich tekstura. */
    void SetHeightmap(const std::vector<std::string>& heightmapsPaths);
    /* Ustawia w shaderach 'shader' oraz 'normalsShader' niektóre zmienne z 
       modyfikatorem uniform. */
    void SetUniforms();

public:
    /* Wyznacza, jak duży obszar kafelka terenu ma być przeznaczony na przekształcanie do wyższego
       poziomu LOD. */
    const float morphRegion = 0.3f;
    // Shader terenu.
    const Shader shader;
    // Shader dla wektorów normalnych terenu.
    const Shader normalsShader;
    
    /* Konstruktor. Przyjmuje jako pierwszy argument kolekcję ścieżek do map wysokości, z których
       utworzona ma być jedna heightmapa. Drugi argument to żądana szerokość siatki terenu. Siatka
       jest kwadratem. */
    LODPlane(const std::vector<std::string>& heightmapsPaths, int planeWidth);
    /* Destruktor. Usuwa obiekty OpenGL utworzone w konstruktorze takie, jak 'heightmapTex',
       'normalMapTex' oraz 'terrainColorsTex'. */
    ~LODPlane();

    /* Rysuje na ekranie całą siatkę terenu z widokiem z kamery 'camera' lub z
       additionalCam, jeśli jest różna od nullptr. */
    void DrawFrom(const MainCamera &camera, const Camera* additionalCam = nullptr) const;
    /* Blokuje poruszanie się siatki razem z kamerą poprzez zamianę wartości flagi
       'meshMovementLocked'. */
    void ToggleMeshMovementLock(MainCamera &mainCam);
    // Zwraca identyfikator tekstury 'normalMapTex'.
    GLuint GetNormalMapTex() const;
    // Zwraca identyfikator tekstury 'heightmapTex'.    
    GLuint GetHeightmapTex() const;
    // Włącza lub wyłącza rysowanie wektorów normalnych wierzchołków terenu.
    void ToggleDebugNormals();
};
