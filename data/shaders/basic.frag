#version 460 core

in vec2 vUV;

out vec4 oColor;

// uniform sampler2D uTexture;

void main() {
	oColor = vec4(0.9, 0.7, 0.1, 1.0); // texture(uTexture, vUV);
}
