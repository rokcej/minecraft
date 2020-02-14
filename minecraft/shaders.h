#pragma once

#include <iostream>
#include <GL/glew.h>

extern const char* vsSource;
extern const char* fsSource;
extern const char* vsSourceTex;
extern const char* fsSourceTex;

GLuint compileProgram(const char* vsSrc, const char* fsSrc);
