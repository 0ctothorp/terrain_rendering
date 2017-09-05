#pragma once

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.hpp"
#include "window.hpp"
#include "frustum.hpp"


// Główna kamera. Widok z niej widzi użytkownik programu.
class MainCamera : public Camera {
private:
    // Ścięty ostrosłup widoku kamery
    Frustum frustum;
    // Kąt obrotu kamery w okół osi Y (poziomy ruch).
    float yaw = glm::radians(0.0f);
    // Kąt obrotu kamery w okół osi X (ruch pionowy).
    float pitch = glm::radians(-25.0f);
    // Wektor określający globalny kierunek w przód.
    glm::vec3 worldFront = glm::vec3(0.0f, 0.0f, -1.0f);
    // Druga pozycja kamery używana, gdy włączona jest blokada poruszania się siatki terenu.
    glm::vec3 position2;
    // Informuje, czy podążanie terenu za kamerą jest włączone.
    bool meshMovementLocked = false;
    glm::vec3 prevPosition;

    // Aktualizuje wektory 'front', 'right' i 'up' po każdej zmianie kątów 'pitch' i 'yaw'.
    void updateCameraVectors();
public:
    // Kąt pola widzenia (field of view).
    const float fov = 60.f;
    // Odległość bliskiej płaszczyzny (near plane) od kamery.
    const float near = 0.01f;
    // Odległość dalekiej płaszczyzny (far plane) od kamery.
    const float far = 10000.0f;
    // Macierz projekcji
    glm::mat4x4 projectionMat = glm::perspective(
        glm::radians(fov), 
        (float)Window::width / (float)Window::height,
        near,
        far
    );
    // Prędkość poruszania się kamery nad terenem.
    float movementSpeed = 100.0f;    
    float lat, lon;

    // 'position' - startowa pozycja kamery.
    MainCamera(glm::vec3 position = glm::vec3(0, 20, 0));
    // Oblicza i zwraca macierz widoku.
    virtual glm::mat4 GetViewMatrix() const override;
    /* Przemieszcza kamerę w kierunku, na który wskazują wciśnięte klawisze klawiatury.
       'keys' - tablica stanu klawiszy. 
       'deltaTime' - czas jaki zajęło wyrenderowanie ostatniej klatki.*/
    void Move(bool *keys, double deltaTime);
    /* Przetwarza ruch myszy odpowiednio poruszając kamerą. 
       'xoffset' - zmiana w położeniu kursora myszy w osi X od ostatniej klatki.
       'yoffset' - jak wyżej, ale dla osi Y. 
       'sensitivity' - czułość myszy */
    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, float sensitivity);
    /* Zmienia szybkość poruszania się kamery.
       'change' - wartość o jaką ma się zmienić. */
    void ChangeMovementSpeed(int change);
    /* Odpowiada na pytanie, czy kafelek o podanych wierzchołkach znajduje się w widoku kamery.
       Ze względu na sposób sprawdzania, niektóre kafelki znajdujące się poza widokiem dadzą
       pozytywną odpowiedź.
       'point*' - jeden z czterech wierzchołków kwadratu, w którym zawiera się kafelek terenu. */
    bool IsInsideFrustum(const glm::vec3 &point1, const glm::vec3 &point2, const glm::vec3 &point3, 
                         const glm::vec3 &point4) const;
    /* Ustawia flagę 'meshMovementLocked' na wartość podaną w argumencie 'locked'. */
    void SetMeshMovementLock(bool locked);

};
