#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "state.h"

#include <src/utils/hash.h>

class Shader;
class Texture;
class Camera;
class Font;
class Text;

class Chunk;

class GameState : public State {
public:
	GameState(Window* window);
	~GameState() override;

	void Update(float dt) override;
	void Render() override;

	void FramebufferSizeCallback(int width, int height) override;
	void KeyCallback(int key, int scancode, int action, int mods) override;
	void CursorPosCallback(double x, double y) override;

private:
	std::unique_ptr<Shader> shader_;
	std::unique_ptr<Texture> texture_;
	std::unordered_map<glm::ivec3, std::unique_ptr<Chunk>, hash::Hash<glm::ivec3>> chunks_;

	std::unique_ptr<Camera> camera_;
	glm::ivec3 input_ = { 0, 0, 0 };
	float mouse_sensitivity_ = 0.001f;

	std::unique_ptr<Shader> text_shader_;
	std::unique_ptr<Font> font_;

	std::unique_ptr<Text> fps_text_;
	int fps_count_ = 0;
	float fps_time_ = 0.0f;
	int fps_ = 0;

};
