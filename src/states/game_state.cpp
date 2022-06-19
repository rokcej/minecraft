#include "game_state.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

GameState::GameState(Window* window) : State(window) {

}

GameState::~GameState() {

}

void GameState::Update(float dt) {

}

void GameState::Render() {
	glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void GameState::FramebufferSizeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	// camera_->SetAspectRatio((float)width / (float)height);
}

void GameState::KeyCallback(int key, int scancode, int action, int mods) {

}

void GameState::CursorPosCallback(double x, double y) {

}
