#pragma once

#include <glad/glad.h>
#include <string>


class TextureParams {
public:
	TextureParams() = default;
	TextureParams(GLuint wrap_s, GLuint wrap_t, GLuint min_filter, GLuint mag_filter);

	GLuint wrap_s_ = GL_CLAMP_TO_EDGE;
	GLuint wrap_t_ = GL_CLAMP_TO_EDGE;
	GLuint min_filter_ = GL_LINEAR;
	GLuint mag_filter_ = GL_LINEAR;
};

class Texture {
public:
	Texture(const std::string& file_path, TextureParams params = TextureParams());
	Texture(int width, int height, int n_channels, unsigned char* data, TextureParams params = TextureParams());
	~Texture();

	void Bind() const;

private:
	void Generate(int width, int height, int n_channels, GLubyte* data, TextureParams params);

private:
	GLuint id_;

	static const int kFallbackWidth;
	static const int kFallbackHeight;
	static const GLubyte kFallbackData[];
};
