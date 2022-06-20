#include "game_state.h"

#include <iostream>
#include <src/gl/shader.h>

GameState::GameState(Window* window) : State(window) {
	program_ = gl::CreateProgram("data/shaders/basic.vert", "data/shaders/basic.frag");

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

    // vao_s store the following calls:
    //   -For vbo_s: glVertexAttribPointer and glEnableVertexAttribArray --> we CAN unbind vbo_s
    //   -For ebo_s: glBindBuffer --> we CAN'T unbind ebo_s
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GameState::~GameState() {
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);
	glDeleteProgram(program_);
}

void GameState::Update(float dt) {

}

void GameState::Render() {
	glClearColor(0.5f, 0.675f, 0.85f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program_);
    glBindVertexArray(vao_);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void GameState::FramebufferSizeCallback(int width, int height) {
	glViewport(0, 0, width, height);
	// camera_->SetAspectRatio((float)width / (float)height);
}

void GameState::KeyCallback(int key, int scancode, int action, int mods) {

}

void GameState::CursorPosCallback(double x, double y) {

}
