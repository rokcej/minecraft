#version 450 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;
out vec2 vTexCoord;

uniform float uInvAspectRatio;

void main() {
	gl_Position = vec4(aPos.x * uInvAspectRatio, aPos.y, 0.f, 1.f);
	vTexCoord = aTexCoord;
}
