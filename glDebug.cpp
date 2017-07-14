#include <vector>
#include <iostream>

#include <GL/glew.h>

#include "glDebug.hpp"


void GetErrors(int line, const char* file) {
    GLenum err;
    std::string errorStr;
    while((err = glGetError()) != GL_NO_ERROR) {
        switch(err) {
        case GL_INVALID_ENUM:
            errorStr = "GL_INVALID_ENUM at ";
            break;
        case GL_INVALID_VALUE:
            errorStr = "GL_INVALID_VALUE at ";
            break;
        case GL_INVALID_OPERATION:
            errorStr = "GL_INVALID_OPERATION at ";
            break;
        case GL_STACK_OVERFLOW:
            errorStr = "GL_STACK_OVERFLOW at ";
            break;
        case GL_STACK_UNDERFLOW:
            errorStr = "GL_STACK_UNDERFLOW at ";
            break;
        case GL_OUT_OF_MEMORY:
            errorStr = "GL_OUT_OF_MEMORY at ";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION at ";
            break;
        case GL_CONTEXT_LOST:
            errorStr = "GL_CONTEXT_LOST at ";
            break;
        case GL_TABLE_TOO_LARGE:
            errorStr = "GL_TABLE_TOO_LARGE at ";
            break;
        default: 
            break;
        }
    }
    if(errorStr.size() > 0)
        std::cerr << errorStr << line << " in " << file << std::endl;
}