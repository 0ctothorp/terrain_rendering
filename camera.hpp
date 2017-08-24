#pragma once

#include <glm/glm.hpp>


// Klasa abstrakcyjna (interfejs), po którym powinny dziedziczyć prawdziwe kamery.
class Camera {
protected:
    // Pozycja kamery w świecie.
    glm::vec3 position;
    // Wektor wskazujący kierunek na wprost od kamery.
    glm::vec3 front;
    // Wektor wskazujący kierunek do góry od kamery.
    glm::vec3 up;
    // Wektor wskazujący kierunek na prawo od kamery.
    glm::vec3 right;
    // Wektor wskazujący globalny kierunek w górę.
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

public:
    // Kąt "field of view".
    static constexpr float fov = 60.f;

    // '_position' - startowa pozycja kamery.
    Camera(glm::vec3 _position);
    // Metoda mająca zwracać macierz widoku.
    virtual glm::mat4 GetViewMatrix() const = 0;
    // Ustawia pozycję kamery na 'pos'.
    void SetPosition(glm::vec3 pos);
    // Zwraca pozycję kamery.
    glm::vec3 GetPosition() const;
    // Zwraca wektor wskazujący kierunek wprost od kamery.
    glm::vec3 GetFront() const;
};
