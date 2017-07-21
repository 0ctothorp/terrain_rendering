## About
Terrain rendering software currently in development.

## Building
Project uses GLM, GLFW and GLEW libraries. On Ubuntu simply install `libglew-dev`, `libglfw3-dev` 
and `libglm-dev`.

To build the project on Linux:

`make`

Execute `make clean` to remove all the object files and the executable.

## Running

Najpiew należy ściągnąć jakieś heightmapy. 

Słuzy do tego skrypt `hgtDownload.sh`. Podajemy mu 2 argumenty. długość i szerokość geograficzną w 1 sringu, np.: `N50E015` (ten ciąg znaków odpowiada temu w jaki sposób są nazwane pliki z mapami wysokości na [tej stronie](https://dds.cr.usgs.gov/srtm/version2_1/SRTM3/Eurasia/)) oraz liczbę naturalną, np. `4`. Oznacza to, że skrypt pobierze kwadrat złożony z 16 map wysokości (4 na 1 boku) zaczynający się w `N50E015`. Przykładowo dla liczby 2 skrypt pobrałby: `N50E015`, `N50E016`, `N49E015`, oraz `N49E016`.

Skrypt sam rozpakowuje pobrane archiwa do folderu `heightmaps`, który również tworzy jeśli taki nie istnieje.

Teraz, aby uruchomić właściwy program należy wywołać polecenie `./main` z odpowiednimi argumentami. 

Podajemy programowi ścieżkę do heightmapy, którą ściągnęliśmy (jesli ściągnęliśmy więcej niż 1 to do tej, którą podawaliśmy jako argument do skryptu pobierającego) oraz liczbę oznaczającą, jak duży kwadrat heightmap chcemy ze sobą złączyć. Ten drugi argument jest opcjonalny. Jeśli nie podamy nic, program założy, że chcemy wyświetlić tylko jedną heightmapę.

Kolejny opcjonalny argument to wielkość siatki terenu. Domyślnie jest to 1024. 

**Kolejność argumentów zarówno dla skryptu, jak i programu jest ważna.**

Przykładowe pierwsze uruchomienie programu może wyglądać tak:
```
$ ./hgtDownload N50E015 4
$ ./main heightmaps/N50E015.hgt 4 4096
```

## Usage

- camera movement:
    - `W` - ahead
    - `A` - left
    - `S` - backwards
    - `D` - right
    - `Space` - up
    - `Left Ctrl` - down
    - mouse - moving camera around
    - `Mouse scroll up` - set faster movement speed
    - `Mouse scroll back` -  set slower movement speed
- `ESC` to get a mouse cursor. Press `ESC` again to lock and hide cursor.
