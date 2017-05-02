#include <vector>
#include <iostream>

#include "glDebug.hpp"

using namespace std;


void GetErrors(int line, const char* file) {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        switch(err) {
        case GL_INVALID_ENUM:
            cerr << "GL_INVALID_ENUM at " << line << " in " << file << endl;
            break;
        case GL_INVALID_VALUE:
            cerr << "GL_INVALID_VALUE at " << line << " in " << file << endl;
            break;
        case GL_INVALID_OPERATION:
            cerr << "GL_INVALID_OPERATION at " << line << " in " << file << endl;
            break;
        case GL_STACK_OVERFLOW:
            cerr << "GL_STACK_OVERFLOW at " << line << " in " << file << endl;
            break;
        case GL_STACK_UNDERFLOW:
            cerr << "GL_STACK_UNDERFLOW at " << line << " in " << file << endl;
            break;
        case GL_OUT_OF_MEMORY:
            cerr << "GL_OUT_OF_MEMORY at " << line << " in " << file << endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cerr << "GL_INVALID_FRAMEBUFFER_OPERATION at " << line << " in " << file << endl;
            break;
        case GL_CONTEXT_LOST:
            cerr << "GL_CONTEXT_LOST at " << line << " in " << file << endl;
            break;
        case GL_TABLE_TOO_LARGE:
            cerr << "GL_TABLE_TOO_LARGE at " << line << " in " << file << endl;
            break;
        default: break;
        }
    }
}