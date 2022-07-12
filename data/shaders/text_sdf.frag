#version 460 core

// Signed distance field (SDF) font rendering
// - https://steamcdn-a.akamaihd.net/apps/valve/2007/SIGGRAPH2007_AlphaTestedMagnification.pdf
// - https://drewcassidy.me/2020/06/26/sdf-antialiasing/
// - https://mortoray.com/2015/06/19/antialiasing-with-a-signed-distance-field/

const float SQRT2_2 = 0.70710678118654757; // 1/sqrt(2)

const float SPREAD = 2.0; // Max SDF value (FreeType default value is 2)
const float SDF_THRESHOLD = 0.166667; // Empirically determined to produce good-looking anti-aliased text

in vec2 vUV;

out vec4 oColor;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main() {
	// Compute the number of texels that are spanned by neighbouring fragments
	// Derived with the help of https://github.com/Chlumsky/msdfgen/issues/22#issuecomment-237003707
	vec2 duv_dx = dFdx(vUV);
	vec2 duv_dy = dFdy(vUV);
	float uvDistance = length(vec2(length(duv_dx), length(duv_dy))) * SQRT2_2;
	float numTexels = uvDistance * textureSize(uTexture, 0).x; // Font atlas is square

	// Spread is defined as a number of pixels in the original font texture
	// If text is rendered at a different resolution than the original font texture, spread needs to be corrected
	float spread = SPREAD / numTexels; // 1/numTexels = fontSize/atlasFontSize
	float sdf = (texture(uTexture, vUV).r * 2.0 - 1.0) * spread;

	// Limit the threshold to prevent background from becoming visible in small text
	float sdfThreshold = min(SDF_THRESHOLD, spread);
	if (sdf < -sdfThreshold) {
		discard;
	}
	float alpha = smoothstep(-sdfThreshold, sdfThreshold, sdf);

	oColor = vec4(uColor.rgb, uColor.a * alpha);
}
