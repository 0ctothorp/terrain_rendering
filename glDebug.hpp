#pragma once


#ifdef DEBUG 
    #define GL_CHECK(x) x; GetFirstNMessages(3, __LINE__, __FILE__);
#else
    #define GL_CHECK(x) x
#endif


/* Pobiera z OpenGL 'numMsgs' komunikatów o błędach. Drugi i trzeci argument służą do przekazania
   numeru linii i nazwy pliku, w którym wykryto błąd (patrz makro powyżej). */
void GetFirstNMessages(GLuint numMsgs, int, const char*);