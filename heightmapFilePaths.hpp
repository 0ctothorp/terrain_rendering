#pragma once

#include <string>
#include <vector>


/* Zwraca wektor ścieżek (w poprawnej kolejności) do map wysokości obliczając ją na postawie
   'heightmapsInRow'. Jeśli 'path' to "N50E015.hgt", a 'heightmapsInRow' = 2, to zwracany vector
   zawierać będzie {"N50E015.hgt", "N50E016.hgt", "N49E015.hgt", "N49E016.hgt"}, czyli ścieżki
   do takich heightmap, które razem tworzą jedną wiekszą heightmapę. 'path' to ścieżka 
   do mapy wysokości, od której ma się zaczynać wyliczanie kolejnych ścieżek. */
std::vector<std::string> GetHeightmapsFilenamesBasedOn(const char* path, int heightmapsInRow);

/* Zwraca nazwę pliku heightmapy na podstawie dostarczonych argumentów.
   Argumenty '(lat|lon)Direction' to literki oznaczające kierunek N, S, W lub E. 
   'lat' należy do [0, 60], a 'lon' - [0, 180]. */
std::string GetHeightmapFileName(int lat, int lon, char latDirection, char lonDirection);

/* Robi to samo co powyższa funkcja, ale na podstawie znaku w 'lat' i 'lon' sama dedykuje
   o jaki kierunek chodzi. Przydatna przy interpretacji inputu z okienka wyboru współrzędnych
   na początku programu. */
std::string GetHeightmapFileNameWithPossibleNegativeLatLon(int lat, int lon);

/* Zwraca wektor nazw plików, które należy ściągnąć spośród podanych jako argument, bo nie
   ma ich na dysku. */
std::vector<std::string> GetFilesToDownloadFrom(const std::vector<std::string>& filenames);