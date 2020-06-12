## About

A program for terrain visualization, which downloads heightmaps for selected area from NASA servers: https://dds.cr.usgs.gov/srtm/.

## Compiling

You'll need to install following libraries to compile the project:
```
libglew-dev libglfw3-dev libglm-dev libcurl3 zlib1g-dev lboost-system-dev lboost-filesystem-dev
```

You'll also need to have Make installed. To build the submodules and the project run:
```
make build-submodules
make
```

## Running

`make run` or `./main`. You can pass 2 arguments which are for width and height of the program window.

## Usage

- Moving around:
    - `WASD` to go in four directions
    - `Space` - go up
    - `Left Ctrl` - go down
    - use mouse to control the camera
    - use mouse scroll to change movement speed
- `ESC` to hide the mouse cursor and show it back again.
