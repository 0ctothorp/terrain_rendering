cmake_minimum_required(VERSION 3.1)
project(TerrainRenderer)

set(TerrainRenderer_VersionMajor 1)
set(TerrainRenderer_VersionMinor 0)

set(CMAKE_CXX_STANDARD 14)

function(add_objs)
    foreach(module ${ARGN})
        add_library(${module} OBJECT ${module}.cpp)
    endforeach(module)
endfunction(add_objs)

set(
    MODULES
    camera
    drawable
    framebuffer
    frustum
    glDebug
    heightmapFilePaths
    hmParser
    lodPlane
    mainCamera
    mouse
    shader
    tileGeometry
    tileMesh
    topCamera
    topViewFb
    topViewScreenQuad
    utils
    window
) 

function(get_sources output_var)
    set(tmpList)
    foreach(module IN LISTS MODULES)
        list(APPEND tmpList ${module}.cpp)
    endforeach(module)
    set(${output_var} ${tmpList} PARENT_SCOPE)
endfunction(get_sources)

get_sources(SOURCES)

# a może zrobić w libs CMakeLists.txt i załączać tylko libs? chyba lepiej.
add_subdirectory(libs)
include_directories(${CPR_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/zipper)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)

add_executable(TerrainRenderer main.cpp ${SOURCES}) 
target_link_libraries(
    TerrainRenderer 
    ${CPR_LIBRARIES}
    ${ZIPPER-LIBRARY}-static
    pthread curl z 
    GL GLEW glfw
)
