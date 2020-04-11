#version 450 core

in vec2 vTexCoord;
out vec4 oColor;
uniform sampler2D uTex;

void main() {
	oColor = texture(uTex, vTexCoord);
}
