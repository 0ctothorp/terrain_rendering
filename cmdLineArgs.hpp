#pragma once

#include <string>
#include <vector>


/* Zwraca kolekcję (w poprawnej kolejności) ścieżek do map wysokości obliczając ją na postawie
   'heightmapsInRow'. Jeśli 'path' to "N50E015.hgt", a 'heightmapsInRow' = 2, to zwracany vector
   zawierać będzie {"N50E015.hgt", "N50E016.hgt", "N49E015.hgt", "N49E016.hgt"}, czyli ścieżki
   do takich heightmap, które razem tworzą jedną wiekszą heightmapę. 'path' to ścieżka 
   do mapy wysokości, od której ma się zaczynać wyliczanie kolejnych ścieżek. */
std::vector<std::string> GetHeightmapsPathsFromCmdLine(const char* path, int heightmapsInRow);