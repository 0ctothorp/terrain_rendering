#pragma once


#ifdef DEBUG 
    #define GL_CHECK(x) x; /* GetErrors(__LINE__, __FILE__) */ GetFirstNMessages(3, __LINE__, __FILE__);
#else
    #define GL_CHECK(x) x
#endif


void GetFirstNMessages(GLuint numMsgs, int, const char*);