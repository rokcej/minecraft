#version 460 core

in vec2 vUV;

out vec4 oColor;

uniform sampler2D uTexture;

void main() {
	oColor = texture(uTexture, vUV);
}
