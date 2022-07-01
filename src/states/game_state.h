#pragma once

#include <memory>
#include <glad/glad.h>
#include "state.h"

class Shader;
class Texture;
class Font;
class Text;

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
	GLuint vbo_, ebo_, vao_;
	std::unique_ptr<Shader> shader_;
	std::unique_ptr<Texture> texture_;

	std::unique_ptr<Shader> text_shader_;
	std::unique_ptr<Font> font_;

	std::unique_ptr<Text> fps_text_;
	int fps_count_ = 0;
	float fps_time_ = 0.0f;
	int fps_ = 0;

};
