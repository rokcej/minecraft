#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;
out vec3 vTexCoord;
uniform mat4 uPVM;

// Fog
out float vDistance;
uniform int uEnableFog;
uniform vec3 uCameraPos;

void main() {
	gl_Position = uPVM * vec4(aPos, 1.0f);
	vTexCoord = aTexCoord;
	
	// Fog
	if (uEnableFog != 0) {
		vDistance = length(aPos - uCameraPos);
	}
}
