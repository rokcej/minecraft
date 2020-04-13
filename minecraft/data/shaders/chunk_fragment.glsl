#version 450 core

in vec3 vTexCoord;
out vec4 oColor;
uniform sampler2DArray uTex;

// Light
in float vIllumination;

// Fog
in float vDistance;
uniform int uEnableFog;
uniform float uRenderDistance;
uniform float uFogDepth;

void main() {
	oColor = texture(uTex, vTexCoord);
	
	// Light
	oColor.rgb *= vIllumination;

	// Fog
	if (uEnableFog != 0) {
		float fogIntensity;
		if (vDistance <= uRenderDistance - uFogDepth) // In front of fog
			fogIntensity = 0.f;
		else if (vDistance >= uRenderDistance) // Behind fog
			fogIntensity = 1.f;
		else // Inside fog
			fogIntensity = pow((vDistance - (uRenderDistance - uFogDepth)) / uFogDepth, 2);
		// Apply fog
		oColor.rgb *= 1 - fogIntensity;
	}
}
