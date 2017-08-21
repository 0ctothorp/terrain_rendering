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

void GetFirstNMessages(GLuint numMsgs, int line, const char* file) {
	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for(size_t msg = 0; msg < lengths.size(); ++msg) {
        std::cerr << file << ":" << line << ">\n";
        std::cerr << std::string(currPos, currPos + lengths[msg] - 1) << std::endl << std::endl;
		currPos = currPos + lengths[msg];
	}
}