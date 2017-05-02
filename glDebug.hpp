#pragma once

#include <GL/glew.h>

// Nalezy tego używać przy każdym wywołaniu funkcji gl*,
// bo inaczej będzie wypisywac błędne numery linii i nazwy plików.
#define GL_CHECK(x) x; GetErrors(__LINE__, __FILE__)


void GetErrors(int, const char*);
