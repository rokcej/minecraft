#pragma once

#include <string>
#include <GL/glew.h>

namespace TextureLoader {
	GLuint loadTextureAtlas(std::string filename);

	GLuint loadTexture(std::string filename);
}
