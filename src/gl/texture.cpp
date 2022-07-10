#include "texture.h"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const int Texture::kFallbackWidth = 2;
const int Texture::kFallbackHeight = 2;
const GLubyte Texture::kFallbackData[] = {
     16,  16,  16, 255, 224,  16, 224, 255,
    224,  16, 224, 255,  16,  16,  16, 255
};

Texture::Texture(int width, int height, int n_channels, GLubyte* data, TextureParams params) {
    Generate(width, height, n_channels, data, params);
}

Texture::Texture(const std::string& file_path, TextureParams params) {
    int width, height, n_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(file_path.c_str(), &width, &height, &n_channels, 0);

    if (data) {
        Generate(width, height, n_channels, data, params);
        stbi_image_free(data);
    } else {
        std::cerr << "[ERROR] Can't load texture " << file_path << std::endl;
        Generate(kFallbackWidth, kFallbackHeight, GL_RGBA, (GLubyte*)kFallbackData, TextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR));
    }
}

Texture::~Texture() {
	glDeleteTextures(1, &id_);
}

void Texture::Generate(int width, int height, int n_channels, GLubyte* data, TextureParams params) {
    width_ = width;
    height_ = height;
    has_mipmap_ = params.generate_mipmap_;

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter_);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, params.max_anisotropy_);

    GLuint format;
    switch (n_channels) {
    case 1: format = GL_RED; break;
    case 3: format = GL_RGB; break;
    case 4: format = GL_RGBA; break;
    default:
        std::cerr << "[ERROR] Unsupported number of channels per pixel: " << n_channels << std::endl;
        format = GL_RED;
        break;
    }
    internal_format_ = format;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (has_mipmap_) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

int Texture::GetWidth() const {
    return width_;
}

int Texture::GetHeight() const {
    return height_;
}

void Texture::SubImage(int x, int y, int width, int height, unsigned char* data) {
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, internal_format_, GL_UNSIGNED_BYTE, data);
    if (has_mipmap_) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, id_);
}


TextureParams::TextureParams(GLuint wrap_s, GLuint wrap_t, GLuint min_filter, GLuint mag_filter, bool generate_mipmap, float max_anisotropy) {
    wrap_s_ = wrap_s;
    wrap_t_ = wrap_t;
    min_filter_ = min_filter;
    mag_filter_ = mag_filter;
    generate_mipmap_ = generate_mipmap;

    GLfloat max_supported_anisotropy;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_supported_anisotropy);
    if (max_anisotropy <= max_supported_anisotropy) {
        max_anisotropy_ = max_anisotropy;
    } else {
        max_anisotropy_ = max_supported_anisotropy;
        std::cerr << "[WARNING] Anisotropy " << max_anisotropy << " exceeds maximum supported anisotropy " << max_supported_anisotropy << std::endl;
    }
}
