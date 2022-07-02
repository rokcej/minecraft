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

GameState::GameState(Window* window) : State(window) {
	window_->SetCursorMode(Window::CursorMode::kDisabled);
	window_->SetCursorPos(0.0, 0.0); // Center cursor for camera movement

	shader_ = std::make_unique<Shader>("data/shaders/basic.vert", "data/shaders/basic.frag");
	texture_ = std::make_unique<Texture>("data/textures/face.png", TextureParams(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_NEAREST)); // GL_LINEAR_MIPMAP_LINEAR

	camera_ = std::make_unique<Camera>();
	camera_->SetAspectRatio((float)window->GetWidth() / (float)window->GetHeight());

	// Data
	const GLfloat vertices[] = {
		// Position       // UV
		// Left
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
		-0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
		-0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
		// Right
		+0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
		+0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		+0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
		+0.5f, +0.5f, +0.5f, 0.0f, 1.0f,
		// Bottom
		+0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		+0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		// Top
		-0.5f, +0.5f, +0.5f, 0.0f, 0.0f,
		+0.5f, +0.5f, +0.5f, 1.0f, 0.0f,
		+0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
		// Back
		+0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, +0.5f, -0.5f, 1.0f, 1.0f,
		+0.5f, +0.5f, -0.5f, 0.0f, 1.0f,
		// Front
		-0.5f, -0.5f, +0.5f, 0.0f, 0.0f,
		+0.5f, -0.5f, +0.5f, 1.0f, 0.0f,
		+0.5f, +0.5f, +0.5f, 1.0f, 1.0f,
		-0.5f, +0.5f, +0.5f, 0.0f, 1.0f
	};

	GLuint indices[] = {
		 0,  1,  2,  2,  3,  0,
		 4,  5,  6,  6,  7,  4,
		 8,  9, 10, 10, 11,  8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20
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

		pos += dt * (norm_input.x * right + norm_input.y * up + norm_input.z * -forward);
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

	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


	// UI
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
