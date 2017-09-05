## O programie

Program do wizualizacji terenu, ściągający i rozpakowujący dane wysokości z serwera NASA: https://dds.cr.usgs.gov/srtm/.

## Kompilacja

Do kompilacji potrzebnych będzie zainstalowanych kilka bibliotek: GLEW, GLFW3, GLM, Boost Filesystem, Boost System, curl oraz zlib. Na systemie Ubuntu wystarczy wywołać poniższą komendę, by zainstalować wymienione oprogramowanie:
```sudo apt-get install libglew-dev libglfw3-dev libglm-dev \ 
libcurl3 zlib1g-dev lboost-system lboost-filesystem```
Poza nimi potrzebne będą również programy GNU Make oraz git. Po zainstalowaniu wymaganego oprogramowania wystarczy uruchomić dwa polecenia:
```make build-submodules
make```

## Uruchomienie

`make run` lub `./main`. Istnieje możliwość przekazania dwóch argumentów oznaczających rozdzielczość okna programu.

## Używanie

- poruszanie kamerą:
    - `W` - w przód
    - `A` - w lewo
    - `S` - w tył
    - `D` - w prawo
    - `Space` - w górę
    - `Left Ctrl` - w dół
    - myszką kontrolujemy obracanie kamery
    - `scroll' - reguluje prędkość poruszania się
- `ESC` chowa kursor myszy lub powoduje pojawienie się go.
