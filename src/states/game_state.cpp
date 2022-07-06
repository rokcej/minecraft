#include "game_state.h"

#include <iostream>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <src/gl/shader.h>
#include <src/gl/texture.h>
#include <src/rendering/camera.h>
#include <src/text/font.h>
#include <src/text/text.h>
#include <src/utils/math.h>
#include <src/world/chunk.h>

GameState::GameState(Window* window) : State(window) {
	window_->SetCursorMode(Window::CursorMode::kDisabled);
	window_->SetCursorPos(0.0, 0.0); // Center cursor for camera movement

	shader_ = std::make_unique<Shader>("data/shaders/basic.vert", "data/shaders/basic.frag");
	texture_ = std::make_unique<Texture>("data/textures/face.png", TextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST)); // GL_LINEAR_MIPMAP_LINEAR

	camera_ = std::make_unique<Camera>();
	camera_->SetAspectRatio((float)window->GetWidth() / (float)window->GetHeight());
	camera_->SetPosition(glm::vec3(0.0f, 1.8f, 0.0f));

	int range = 5;
	for (int x = -range; x <= range; ++x) {
		for (int z = -range; z <= range; ++z) {
			for (int y = -range; y < 0; ++y) {
				chunks_.push_back(new Chunk(glm::vec3(x, y, z) * 16.0f));
			}
		}
	}

	// Text
	text_shader_ = std::make_unique<Shader>("data/shaders/text.vert", "data/shaders/text.frag");
	font_ = std::make_unique<Font>("data/fonts/Roboto-Regular.ttf", 24);
	fps_text_ = std::make_unique<Text>("0 FPS", font_.get());
}

GameState::~GameState() {

}

void GameState::Update(float dt) {
	// Input
	if (input_ != glm::ivec3(0)) {
		glm::vec3 norm_input = glm::normalize(glm::vec3(input_));

		glm::vec3 pos = camera_->GetPosition();
		glm::vec3 rot = camera_->GetRotation();

		float cos_rot_x = std::cos(rot.x);
		glm::vec3 forward(
			-std::sin(rot.y) * cos_rot_x,
			 std::sin(rot.x),
			-std::cos(rot.y) * cos_rot_x
		);
		glm::vec3 up(0.0f, 1.0f, 0.0f);
		glm::vec3 right = glm::cross(forward, up);

		pos += dt * (norm_input.x * right + norm_input.y * up + norm_input.z * -forward) * 5.0f;
		camera_->SetPosition(pos);
	}

	// 3D
	camera_->Update();

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

	for (const Chunk* chunk : chunks_) {
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
	}
}

void GameState::CursorPosCallback(double x, double y) {
	glm::vec3 rot = camera_->GetRotation();

	rot.x -= (float)y * mouse_sensitivity_; // Pitch
	rot.y -= (float)x * mouse_sensitivity_; // Yaw

	// Wrap yaw to [-pi, pi]
	rot.y = std::fmodf(rot.y + math::kPi, 2.0f * math::kPi) - math::kPi;

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
