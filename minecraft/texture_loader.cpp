#include "texture_loader.h"

#include "block_data.h"
#include "png_reader.h"

GLuint TextureLoader::loadTextureAtlas(const char* filename) {
	// Read image
	int imageWidth, imageHeight, imageChannels = 4;
	GLubyte* imageData = read_png("data/textures.png", &imageWidth, &imageHeight, false);
	if (imageData == nullptr)
		return 0;
	GLenum imageFormat = imageChannels == 4 ? GL_RGBA : GL_RGB;
	int textureWidth = imageWidth / TEXTURE_ATLAS_WIDTH;
	int textureHeight = imageHeight / TEXTURE_ATLAS_HEIGHT;
	int numTextures = TEXTURE_ATLAS_WIDTH * TEXTURE_ATLAS_HEIGHT;

	// Create array texture
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, tex);
	
	// Texture parameters
	/// Texture filtering
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/// Mipmaps
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 4);
	/// Anisotropic filtering
	GLfloat maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);
	/// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Load texture data
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, textureWidth, textureHeight, numTextures, 0, imageFormat, GL_UNSIGNED_BYTE, NULL);
	/// Read each tile and store it into a layer of the array texture
	GLubyte* textureData = new GLubyte[(size_t)textureWidth * textureHeight * imageChannels];
	for (int textureY = 0; textureY < TEXTURE_ATLAS_HEIGHT; ++textureY) {
		for (int textureX = 0; textureX < TEXTURE_ATLAS_WIDTH; ++textureX) {
			// Read texture data (invert horizontally to match OpenGL UV coordinates)
			GLubyte* ptr = imageData + (
				(((size_t)textureY + 1) * textureHeight - 1) * textureWidth * TEXTURE_ATLAS_WIDTH + // Row (start at the bottom)
				(size_t)textureX * textureWidth // Column
				) * imageChannels;
			for (int texelY = 0; texelY < textureHeight; ++texelY) {
				for (int texelX = 0; texelX < textureWidth; ++texelX) {
					for (int channel = 0; channel < imageChannels; ++channel) {
						int texelIndex = texelY * textureWidth + texelX;
						textureData[texelIndex * imageChannels + channel] = *(ptr++); // Move to next column
					}
				}
				ptr -= (size_t)textureWidth * (TEXTURE_ATLAS_WIDTH + 1) * imageChannels; // Move to previous row
			}
			// Insert texture data into array texture
			int textureIndex = textureY * TEXTURE_ATLAS_WIDTH + textureX;
			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, textureIndex, textureWidth, textureHeight, 1, imageFormat, GL_UNSIGNED_BYTE, textureData);
		}
	}

	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	// Free memory
	delete[] textureData;
	free_png(imageData);

	// Return texture id
	return tex;
}
