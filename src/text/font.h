#pragma once

#include <memory>
#include <string>
#include <map>
#include <glm/glm.hpp>

class Texture;

struct FontCharacter {
	glm::vec2 uv_min_;
	glm::vec2 uv_max_;
	glm::vec2 size_;
	glm::vec2 bearing_;
	float advance_;
};


class Font {
public:
	Font(const std::string& file_path, int nominal_height);
	Font(const std::string& file_path, int atlas_width, int atlas_height);
	~Font();

	const FontCharacter& GetCharacter(char c) const;
	const std::unique_ptr<Texture>& GetAtlas() const;

private:
	std::map<unsigned char, FontCharacter> characters_;
	std::unique_ptr<Texture> atlas_ = nullptr;

	static const int kNumChars;
	static const int kFirstChar;

};
