#pragma once

#include <GL/glew.h>

extern const char* vsSource;
extern const char* fsSource;
extern const char* vsSourceTex;
extern const char* fsSourceTex;

extern const char* vsSourceLine;
extern const char* fsSourceLine;

GLuint compileProgram(const char* vsSrc, const char* fsSrc);
