#pragma once

#define DATA_PATH "data/"
#define SHADER_DATA_PATH (DATA_PATH "shaders/")
#define TEXTURE_DATA_PATH DATA_PATH

enum Side {
	LEFT = 0, // -x
	RIGHT,    // +x
	BOT,      // -y
	TOP,      // +y
	BACK,     // -z
	FRONT     // +z
};
