#version 460 core

layout (location = 0) in vec4 aData; // [x, y, u, v]

out vec2 vUV;

uniform mat4 uProjMat;

void main() {
	gl_Position = uProjMat * vec4(aData.xy, 0.0f, 1.0);
	vUV = aData.zw;
}
