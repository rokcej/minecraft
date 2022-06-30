#pragma once

#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>

class Texture;

class FontCharacter {
public:
	FontCharacter() = default;
	FontCharacter(glm::vec2 uv_min, glm::vec2 uv_max, glm::ivec2 size, glm::ivec2 bearing, unsigned int advance);

	glm::vec2 uv_min_;
	glm::vec2 uv_max_;
	glm::ivec2 size_;
	glm::ivec2 bearing_;
	unsigned int advance_;

};


class Font {
public:
	Font(const std::string& file_path, int height);
	~Font();

public: // TODO: Make private
	FontCharacter characters_[128];
	std::unique_ptr<Texture> atlas_ = nullptr;

};
