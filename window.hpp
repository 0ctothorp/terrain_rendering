#pragma once

// Klasa pozwalająca na globalny dostęp do szerokości i wysokości okna programu.
class Window {
private:
    Window() {}
public:
    static int width;
    static int height;
};