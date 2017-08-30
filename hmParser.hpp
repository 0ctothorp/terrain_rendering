#pragma once

#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>


/* Klasa służąca do parsowania map wysokości w formacie .hgt SRTM3, które można ściągnąć z 
   https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/ i łączenia wielu takich plików w 
   jedną heightmapę. */
class HMParser {
private:
    // Pomocnicza enumeracja służąca do określania kierunków.
    enum PointTo_ {
        PointTo_Right,
        PointTo_Left,
        PointTo_Bottom,
        PointTo_Top,
        PointTo_BottomRight,
        PointTo_BottomLeft,
        PointTo_TopRight,
        PointTo_TopLeft
    };

    // Szerokość heightmapy po połączeniu wszystkich plików w całość
    int totalWidth;
    // Dane scalonej heightmapy
    std::vector<short> data;
    // Dane wektorów normalnych dla każdego wierzchołka scalonej heightmapy.
    std::vector<char> normals;
    // Kolekcja zawierająca pliki heightmap.
    std::vector<std::ifstream> heightmapFiles;
    // Kolekcja zawierająca ścieżki do map wysokości.
    const std::vector<std::string> heightmapPaths;
    // Wartości właściwych wysokości dla każdego wierzchołka siatki terenu.
    std::vector<float> heights;

    /* Próbuje otworzyć plik pod ścieżką 'path' używając strumienia 'stream'.
       Jeśli się nie uda kończy pracę programu. */
    void TryToOpenAFile(const std::string& path, std::ifstream& stream);
    /* Czyta każdą heightmapę z 'heightmapFiles' i zapisuje odczytane dane do 'data', 
       dbając o to, by pliki zostały odpowiednio połączone. */
    void ParseHeightmaps();
    // Próbuje odczytać wiersz danych z 'heightmapFiles[heightmapsInRow * rowNumber + positionInRow]'.
    void TryToReadRowOfDataFromFile(int heightmapsInRow, int rowNumber, int positionInRow);
    // Zamyka wszystkie otwarte pliki (czyli te w 'heightmapFiles').
    void CloseFiles();
    /* Oblicza wektory normalne dla każdego wierzchołka wirtualnej siatki terenu, która ma
       tyle wierzchołków ile jest łącznie danych w heightmapach. */
    void CalculateNormals();
    // Zwraca wektor normalny dla trójkąta o wierzchołkach w 'point', 'p1', 'p2'.
    glm::vec3 GetTriangleNormal(glm::vec3 point, glm::vec3 p1, glm::vec3 p2);
    // Zwraca punkt w kierunku 'pointTo' od 'point'. 
    glm::vec3 GetPointTo(glm::vec3 point, PointTo_ pointTo);
    // Odwraca bajty w zmiennej 's', ponieważ wymagają tego dane w plikach .hgt.
    short SwapBytes(short s);
    /* Zwraca wektor normalny dla trójkąta znajdującego się w kierunku wskazywanym przez 'dir' od
       'point'. */
    glm::vec3 GetTriangleNormalTo(glm::vec3 point, PointTo_ dir);
    /* Oblicza wektory normalne dla każdego trójkąta wirtualnej siatki, która ma tyle wierzchołków
       ile jest łącznie danych w heightmapach. */
    std::vector< std::vector<glm::vec3> > CalculateTriangleNormals();
    /* Zamień bajty wszystkim wartościom w 'data' po to, by nie robić tego wielokrotnie podczas
       obliczania normalnych. */
    void SwapBytesForAllValuesInHeightmap();
    /* Zwraca końcową wartość wysokości (taką która pojawi się w shaderze) dla wysokości 
       odczytanej z heightmapy. */
    float GetHeight(short h);
    /* Eliminuje braki danych w heightmapie */
    void EliminateDataVoids();

public:
    // Szerokość jednej heightmapy typu SRTM3, czyli ile wartości mieści się "na jednym boku".
    const int width = 1201;
    
    // 'heightmaps' - ścieżki do plików map wysokości.
    HMParser(const std::vector<std::string>& heightmaps);
    // Zwraca wskaźnik do złączonej mapy wysokości.
    std::vector<short>* GetDataPtr();
    // Zwraca wskaźnik do normalnych.
    std::vector<char>* GetNormalsPtr();
    // Zwraca wartość 'totalWidth'.
    int GetTotalWidth();
};