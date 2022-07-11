#include "game_state.h"

#include <iostream>
#include <format>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <src/gl/shader.h>
#include <src/gl/texture.h>
#include <src/rendering/camera.h>
#include <src/text/font.h>
#include <src/text/text.h>
#include <src/utils/math.h>

#include <src/world/chunk_manager.h>
#include <src/world/chunk.h>

GameState::GameState(Window* window) : State(window) {
	window_->SetCursorMode(Window::CursorMode::kDisabled);
	window_->SetCursorPos(0.0, 0.0); // Center cursor for camera movement

	shader_ = std::make_unique<Shader>("data/shaders/basic.vert", "data/shaders/basic.frag");
	texture_ = std::make_unique<Texture>("data/textures/face.png", TextureParams(GL_REPEAT, GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST, true, 16.0f));

	camera_ = std::make_unique<Camera>();
	camera_->SetAspectRatio((float)window->GetWidth() / (float)window->GetHeight());
	camera_->SetPosition(glm::vec3(0.0f, 1.8f, 0.0f));

	chunk_manager_ = std::make_unique<ChunkManager>();

	// Text
	text_shader_ = std::make_unique<Shader>("data/shaders/text.vert", "data/shaders/text.frag");
	font_ = std::make_unique<Font>("data/fonts/Roboto-Regular.ttf", 24);
	fps_text_ = std::make_unique<Text>("0 FPS", font_.get());
	debug_text_ = std::make_unique<Text>("", font_.get());
}

GameState::~GameState() {

}

void GameState::Update(float dt) {
	// Input
	if (input_ != glm::ivec3(0)) {
		glm::vec3 norm_input = glm::normalize(glm::vec3(input_));

		glm::vec3 pos = camera_->GetPosition();
		glm::vec3 rot = camera_->GetRotation();

		glm::vec3 forward = camera_->GetForward();
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::cross(forward, up);

		float speed = sprinting_ ? 20.0f : 5.0f;
		pos += speed * dt * (norm_input.x * right + norm_input.y * up + norm_input.z * -forward);
		camera_->SetPosition(pos);
	}

	// 3D
	camera_->Update();
	chunk_manager_->Update(camera_->GetPosition());

	// FPS
	++fps_count_;
	fps_time_ += dt;
	if (fps_time_ >= 1.0f) {
		fps_ = (int)std::round((float)fps_count_ / fps_time_);
		fps_text_->SetText(std::to_string(fps_) + " FPS");
		fps_count_ = 0;
		fps_time_ = 0.0f;
	}

	// Debugging
	if (show_debug_info_) {
		glm::vec3 pos = camera_->GetPosition();
		glm::ivec3 chunk_pos(glm::floor(pos / (float)Chunk::kSize));
		glm::vec3 dir = camera_->GetForward();
		debug_text_->SetText(
			std::format("XYZ: {:.4f} / {:.4f} / {:.4f}\n", pos.x, pos.y, pos.z) +
			std::format("Chunk: {}, {}, {}\n", chunk_pos.x, chunk_pos.y, chunk_pos.z) +
			std::format("Direction: {:.2f}, {:.2f}, {:.2f}", dir.x, dir.y, dir.z)
		);
	}
}

void GameState::Render() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.5f, 0.675f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// 3D
	shader_->Use();
	glActiveTexture(GL_TEXTURE0);
	texture_->Bind();

	glm::mat4 model_mat(1.0f);
	model_mat = glm::translate(model_mat, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 pvm_mat = camera_->GetProjViewMat() * model_mat;
	shader_->SetMatrix4("uPVMMat", pvm_mat);

	for (const auto& [_, chunk] : chunk_manager_->GetChunks()) {
		glBindVertexArray(chunk->vao_);
		glDrawElements(GL_TRIANGLES, chunk->num_indices_, GL_UNSIGNED_INT, 0);
	}


	// UI
	glClear(GL_DEPTH_BUFFER_BIT);
	text_shader_->Use();
	glActiveTexture(GL_TEXTURE0);

	glm::mat4 ui_proj_mat = glm::ortho(0.0f, (float)window_->GetWidth(), 0.0f, (float)window_->GetHeight(), -1.0f, 1.0f);
	text_shader_->SetMatrix4("uProjMat", ui_proj_mat);

	fps_text_->SetPosition(glm::vec2(8.0f, window_->GetHeight() - fps_text_->GetHeight() - 4.0f));
	fps_text_->Render(text_shader_);

	if (show_debug_info_) {
		debug_text_->SetPosition(glm::vec2(8.0f, window_->GetHeight() - 3 * fps_text_->GetHeight() - 4.0f));
		debug_text_->Render(text_shader_);
	}
}

void GameState::FramebufferSizeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	camera_->SetAspectRatio((float)width / (float)height);
}

void GameState::KeyCallback(int key, int scancode, int action, int mods) {
	int input_diff = 0;
	if (action == GLFW_PRESS) {
		input_diff = 1;
	} else if (action == GLFW_RELEASE) {
		input_diff = -1;
	}

	switch (key) {
		// Options
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS) {
			window_->Close();
		}
		break;
	case GLFW_KEY_F3:
		if (action == GLFW_PRESS) {
			show_debug_info_ = !show_debug_info_;
		}
		break;
		// Movement
	case GLFW_KEY_W:
		input_.z -= input_diff;
		break;
	case GLFW_KEY_S:
		input_.z += input_diff;
		break;
	case GLFW_KEY_A:
		input_.x -= input_diff;
		break;
	case GLFW_KEY_D:
		input_.x += input_diff;
		break;
	case GLFW_KEY_SPACE:
		input_.y += input_diff;
		break;
	case GLFW_KEY_LEFT_CONTROL:
		input_.y -= input_diff;
		break;
		// Modifiers
	case GLFW_KEY_LEFT_SHIFT:
		if (action == GLFW_PRESS) {
			sprinting_ = true;
		} else if (action == GLFW_RELEASE) {
			sprinting_ = false;
		}
		break;
	}
}

void GameState::CursorPosCallback(double x, double y) {
	glm::vec3 rot = camera_->GetRotation();

	rot.x -= (float)y * mouse_sensitivity_; // Pitch
	rot.y -= (float)x * mouse_sensitivity_; // Yaw

	// Wrap yaw to [-pi, pi]
	rot.y = std::fmod(rot.y + math::kPi, 2.0f * math::kPi) - math::kPi;

	// Limit pitch to [-pi/2, pi/2]
	// TODO: Remove epsilon correction
	const float epsilon = 0.1f;
	if (rot.x > math::kPiHalf - epsilon) {
		rot.x = math::kPiHalf - epsilon;
	}
	else if (rot.x < -math::kPiHalf + epsilon) {
		rot.x = -math::kPiHalf + epsilon;
	}

	camera_->SetRotation(rot);
	window_->SetCursorPos(0.0, 0.0); // Center cursor for camera movement
}
