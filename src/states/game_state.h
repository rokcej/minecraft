#pragma once

#include "state.h"

class GameState : public State {
public:
	GameState(Window* window);
	~GameState() override;

	void Update(float dt) override;
	void Render() override;

	void FramebufferSizeCallback(int width, int height) override;
	void KeyCallback(int key, int scancode, int action, int mods) override;
	void CursorPosCallback(double x, double y) override;

};
