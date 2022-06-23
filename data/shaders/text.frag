#version 460 core

in vec2 vUV;

out vec4 oColor;

uniform sampler2D uTexture;
uniform vec3 uColor;

void main() {
	float alpha = texture(uTexture, vUV).r;
	oColor = vec4(uColor, alpha);
}
