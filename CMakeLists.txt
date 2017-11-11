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
    main
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
add_executable(TerrainRenderer ${SOURCES}) 

add_subdirectory(libs)

include_directories(${IMGUI_INCLUDE_DIR})
include_directories(${ZIPPER_INCLUDE_DIR})
include_directories(${CPR_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS})

find_package(glfw3 REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${BOOST_INCLUDE_DIR})

target_link_libraries(
    TerrainRenderer 
    curl 
    pthread 
    z
    ${OPENGL_gl_LIBRARY}
    ${GLEW_LIBRARIES} 
    ${GLFW3_LIBRARY}
    ${CPR_LIBRARIES}
    imgui
    Zipper-static
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
)

