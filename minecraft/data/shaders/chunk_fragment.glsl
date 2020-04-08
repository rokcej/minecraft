#version 450 core

in vec3 vTexCoord;
out vec4 oColor;
uniform sampler2DArray uTex;

// Fog
in float vFogIntensity;
uniform int uEnableFog;

void main() {
	oColor = texture(uTex, vTexCoord);
	
	// Apply fog
	if (uEnableFog != 0)
		oColor.rgb *= 1 - vFogIntensity;
}
