#include "game_state.h"

#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <src/gl/shader.h>
#include <src/gl/texture.h>
#include <src/text/font.h>
#include <src/text/text.h>

GameState::GameState(Window* window) : State(window) {
	shader_ = std::make_unique<Shader>("data/shaders/basic.vert", "data/shaders/basic.frag");
	texture_ = std::make_unique<Texture>("data/textures/face.png", TextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST)); // GL_LINEAR_MIPMAP_LINEAR

	const GLfloat vertices[] = {
		// Position         // UV
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		+0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		+0.5f, +0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, +0.5f, 0.0f, 0.0f, 1.0f
	};

	GLuint indices[] = {
		 0,  1,  2,  2,  3,  0
	};

	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glGenBuffers(1, &ebo_);

	glBindVertexArray(vao_);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// VAOs store the following calls:
	//   -For VBOs: glVertexAttribPointer and glEnableVertexAttribArray --> we CAN unbind VBOs
	//   -For EBOs: glBindBuffer --> we CAN'T unbind EBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Text
	text_shader_ = std::make_unique<Shader>("data/shaders/text.vert", "data/shaders/text.frag");
	font_ = std::make_unique<Font>("data/fonts/Roboto-Regular.ttf", 24);
	fps_text_ = std::make_unique<Text>("0 FPS", font_.get());
}

GameState::~GameState() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
}

void GameState::Update(float dt) {
	// FPS
	++fps_count_;
	fps_time_ += dt;
	if (fps_time_ >= 1.0f) {
		fps_ = (int)std::roundf((float)fps_count_ / fps_time_);
		fps_text_->SetText(std::to_string(fps_) + " FPS");
		fps_count_ = 0;
		fps_time_ = 0.0f;
	}
}

void GameState::Render() {
	glClearColor(0.5f, 0.675f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	// 3D
	shader_->Use();
	glActiveTexture(GL_TEXTURE0);
	texture_->Bind();

	glm::mat4 proj_mat = glm::ortho(0.0f, (float)window_->GetWidth(), 0.0f, (float)window_->GetHeight(), -1.0f, 1.0f);
	glm::mat4 model_mat(1.0f);
	model_mat = glm::translate(model_mat, glm::vec3(640.0f, 360.0f, 0.0f));
	model_mat = glm::scale(model_mat, glm::vec3(256.0f, 256.0f, 1.0f));
	glm::mat4 pvm_mat = proj_mat * model_mat;
	shader_->SetMatrix4("uPVMMat", pvm_mat);

	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	// UI
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	text_shader_->Use();
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 ui_proj_mat = glm::ortho(0.0f, (float)window_->GetWidth(), 0.0f, (float)window_->GetHeight(), -1.0f, 1.0f);
	text_shader_->SetMatrix4("uProjMat", ui_proj_mat);

	fps_text_->SetPosition(glm::vec2(8.0f, window_->GetHeight() - fps_text_->GetHeight() - 4.0f));
	fps_text_->Render(text_shader_);
}

void GameState::FramebufferSizeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	// camera_->SetAspectRatio((float)width / (float)height);
}

void GameState::KeyCallback(int key, int scancode, int action, int mods) {

}

void GameState::CursorPosCallback(double x, double y) {

}
