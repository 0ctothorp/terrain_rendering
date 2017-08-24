#pragma once

#include <array>
#include <glm/glm.hpp>

#include "camera.hpp"


/* Klasa do reprezentacji ściętego ostrosłupa widoku kamery. */
class Frustum {
private: 
    // Współczyniki równań sześciu płaszczyzn ściętego ostrosłupa.
    std::array<glm::vec4, 6> planes;
    // Wskaźnik do macierzy projekcji kamery, która korzysta z obiektu tej klasy.
    const glm::mat4x4 *projMat;
    // Wskaźnik do kamery, której tyczy się "frustum".
    const Camera *camera;
    enum Side {left = 0, right, top, bottom, near, far};

    /* Zwraca 1, gdy 'point' leży po dobrej stronie płaszczyzny, czyli po tej, w którą skierowany
       jest jej wektor normalny; -1 w przeciwnym przypadku i 0, gdy 'point' leży dokładnie na
       płaszczyźnie określonej przez 'plane'. */
    int SignedDistToPoint(int plane, const glm::vec3 &point) const;
public:
    /* 'camera' - wskaźnik do kamery, której tyczy się "frustum".
       'projMat' - macierz projekcji kamery. */
    Frustum(const Camera* camera, const glm::mat4x4* projMat);
    // "Wyciąga" współczynniki równań płaszczyzn "frustum" z macierzy projekcji.
    void ExtractPlanes();
    /* Odpowiada na pytanie, czy kostka złożona z punktów w 'points' znajduje się w 
       w środku tego "frustum". */
    bool IsCubeInside(const std::array<glm::vec3, 8> &points) const;
};