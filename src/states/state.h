#pragma once

#include <src/window.h>

class State {
public:
	State(Window* window);
	virtual ~State();

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	virtual void FramebufferSizeCallback(int width, int height) {}
	virtual void KeyCallback(int key, int scancode, int action, int mods) {}
	virtual void CursorPosCallback(double x, double y) {}

protected:
	Window* window_;
};
