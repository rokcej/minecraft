#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

uint8_t* read_png(const char* filename, int* w, int* h);
uint8_t* read_png(const char* filename, int* w, int* h, bool flip_vertically);
uint8_t* _read_png(FILE* fp, uint32_t* w, uint32_t* h, bool flip_vertically);
void free_png(uint8_t* image_data);
