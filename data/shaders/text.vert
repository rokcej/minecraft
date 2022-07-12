#version 460 core

layout (location = 0) in vec4 aData; // [x, y, u, v]

out vec2 vUV;

uniform mat4 uProjMat;
uniform vec2 uPos;
uniform float uSize;

void main() {
	gl_Position = uProjMat * vec4(aData.xy * uSize + uPos, 0.0f, 1.0);
	vUV = aData.zw;
}
