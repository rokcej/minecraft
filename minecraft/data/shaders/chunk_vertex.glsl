#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;
layout (location = 2) in vec3 aNormal;
out vec3 vTexCoord;
uniform mat4 uPVM;

// Light
out float vIllumination;

// Fog
out float vDistance;
uniform int uEnableFog;
uniform vec3 uCameraPos;

void main() {
	gl_Position = uPVM * vec4(aPos, 1.0f);
	vTexCoord = aTexCoord;
	
	// Light
	vIllumination = (dot(aNormal, vec3(0.f, 1.f, 0.f)) * .5f + .5f) * .25f + .75f;

	// Fog
	if (uEnableFog != 0) {
		vDistance = length(aPos - uCameraPos);
	}
}
