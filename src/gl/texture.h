#pragma once

#include <glad/glad.h>
#include <string>


class TextureParams {
public:
	TextureParams() = default;
	TextureParams(GLuint wrap_s, GLuint wrap_t, GLuint min_filter, GLuint mag_filter, bool generate_mipmap = false, float max_anisotropy = 1.0f);

	GLuint wrap_s_ = GL_CLAMP_TO_EDGE;
	GLuint wrap_t_ = GL_CLAMP_TO_EDGE;
	GLuint min_filter_ = GL_LINEAR;
	GLuint mag_filter_ = GL_LINEAR;
	bool generate_mipmap_ = false;
	float max_anisotropy_ = 1.0f;
};

class Texture {
public:
	Texture(const std::string& file_path, TextureParams params = TextureParams());
	Texture(int width, int height, int n_channels, unsigned char* data, TextureParams params = TextureParams());
	~Texture();

	int GetWidth() const;
	int GetHeight() const;

	void Bind() const;
	void SubImage(int x, int y, int width, int height, unsigned char* data);

private:
	void Generate(int width, int height, int n_channels, GLubyte* data, TextureParams params);

private:
	GLuint id_;
	GLuint internal_format_;
	int width_;
	int height_;
	bool has_mipmap_;

	static const int kFallbackWidth;
	static const int kFallbackHeight;
	static const GLubyte kFallbackData[];
};
