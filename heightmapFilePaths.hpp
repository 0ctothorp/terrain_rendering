#pragma once

#include <string>
#include <vector>


/* Zwraca wektor nazw plików map wysokości zaczynający się od pliku zawierającego
   dane dla długości 'lat' i szerokości 'lon'. */
std::vector<std::string> GetHeightmapsFilenamesBasedOn(int lat, int lon, int heightmapsInRow);

/* Zwraca nazwę pliku heightmapy na podstawie dostarczonych argumentów.
   Argumenty '(lat|lon)Direction' to literki oznaczające kierunek N, S, W lub E. 
   'lat' należy do [0, 60], a 'lon' - [0, 180]. */
std::string GetHeightmapFileName(int lat, int lon, char latDirection, char lonDirection);

/* Zwraca wektor nazw plików, które należy ściągnąć spośród podanych jako argument, bo nie
   ma ich na dysku. */
std::vector<std::string> GetFilesToDownloadFrom(const std::vector<std::string>& filenames);