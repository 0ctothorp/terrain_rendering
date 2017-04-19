#include <vector>
#include <iostream>

#include "glDebug.hpp"

using namespace std;


void GetFirstNMessages(GLuint numMsgs) {
    GLint maxMsgLen = 0;
    glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

    std::vector<GLchar> msgData(numMsgs * maxMsgLen);
    std::vector<GLenum> sources(numMsgs);
    std::vector<GLenum> types(numMsgs);
    std::vector<GLenum> severities(numMsgs);
    std::vector<GLuint> ids(numMsgs);
    std::vector<GLsizei> lengths(numMsgs);

    GLuint numFound = glGetDebugMessageLog(numMsgs, 1000, &sources[0], &types[0], &ids[0], 
                                           &severities[0], &lengths[0], &msgData[0]);

    sources.resize(numFound);
    types.resize(numFound);
    severities.resize(numFound);
    ids.resize(numFound);
    lengths.resize(numFound);

    std::vector<std::string> messages;
    messages.reserve(numFound);

    std::vector<GLchar>::iterator currPos = msgData.begin();
    for(size_t msg = 0; msg < lengths.size(); ++msg) {
        messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
        currPos = currPos + lengths[msg];
        std::cout << "[GL DEBUG MESSAGE] " << messages[msg] << '\n';
    }
}

void GetErrors() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        switch(err) {
        case GL_INVALID_ENUM:
            cerr << "GL_INVALID_ENUM" << endl;
            break;
        case GL_INVALID_VALUE:
            cerr << "GL_INVALID_VALUE" << endl;
            break;
        case GL_INVALID_OPERATION:
            cerr << "GL_INVALID_OPERATION" << endl;
            break;
        case GL_STACK_OVERFLOW:
            cerr << "GL_STACK_OVERFLOW" << endl;
            break;
        case GL_STACK_UNDERFLOW:
            cerr << "GL_STACK_UNDERFLOW" << endl;
            break;
        case GL_OUT_OF_MEMORY:
            cerr << "GL_OUT_OF_MEMORY" << endl;
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl;
            break;
        case GL_CONTEXT_LOST:
            cerr << "GL_CONTEXT_LOST" << endl;
            break;
        case GL_TABLE_TOO_LARGE:
            cerr << "GL_TABLE_TOO_LARGE" << endl;
            break;
        default: break;
        }
    }
}