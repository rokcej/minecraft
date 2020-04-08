#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoord;
out vec3 vTexCoord;
uniform mat4 uPVM;

// Fog
out float vFogIntensity;
uniform int uEnableFog;
uniform vec3 uCameraPos;
uniform float uRenderDistance;
uniform float uFogDepth;

void main() {
	gl_Position = uPVM * vec4(aPos, 1.0f);
	vTexCoord = aTexCoord;
	
	// Calculate fog
	if (uEnableFog != 0) {
		float distance = length(aPos - uCameraPos);
		if (distance <= uRenderDistance - uFogDepth) // In front of fog
			vFogIntensity = 0.f;
		else if (distance >= uRenderDistance) // Behind fog
			vFogIntensity = 1.f;
		else // Inside fog
			vFogIntensity = pow((distance - (uRenderDistance - uFogDepth)) / uFogDepth, 2);
	}
}
