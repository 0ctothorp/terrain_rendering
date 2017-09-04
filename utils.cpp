#include <cstdlib>

#include "utils.hpp"


int random(int min, int max) {
    float rand = std::rand() / (float)RAND_MAX;
    return min + (max - min) * rand;
}