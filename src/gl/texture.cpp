#include "texture.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int Texture::kFallbackWidth = 2;
const int Texture::kFallbackHeight = 2;
const GLubyte Texture::kFallbackData[] = {
     16,  16,  16, 224,  16, 224, 0, 0, // 2 bytes for alignment
    224,  16, 224,  16,  16,  16
};

Texture::Texture(const std::string& file_path) {
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, n_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* texture_data = stbi_load(file_path.c_str(), &width, &height, &n_channels, 0);

    if (texture_data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "[ERROR] Can't load texture " << file_path << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, kFallbackWidth, kFallbackHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, kFallbackData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(texture_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
	glDeleteTextures(1, &id_);
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, id_);
}
