add_library(
    imgui 
    imgui.cpp 
    imgui_draw.cpp 
    imgui_impl_glfw_gl3.cpp
)

set(IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
