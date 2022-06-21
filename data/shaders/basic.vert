#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;

out vec2 vUV;

uniform mat4 uPVMMat;

void main() {
	gl_Position = uPVMMat * vec4(aPos, 1.0);
	vUV = aUV;
}
