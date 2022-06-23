#pragma once

#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>

class Texture;

class FontCharacter {
public:
	FontCharacter() = default;
	FontCharacter(std::unique_ptr<Texture> texture, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance);

	std::unique_ptr<Texture> texture_ = nullptr;
	glm::ivec2 size_;
	glm::ivec2 bearing_;
	unsigned int advance_;

};


class Font {
public:
	Font(const std::string& file_path, int height);
	~Font();

public:
	std::map<unsigned char, FontCharacter> characters_;

};
