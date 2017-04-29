#version 430

in GS_OUT {
	vec3 normal;
  vec3 worldSpaceCoordinates;
} gs_out;

out vec4 fragColor;

uniform sampler2D rockTextureX;
uniform sampler2D rockTextureY;
uniform sampler2D rockTextureZ;

void main()
{
  // fragColor = vec4(gs_out.normal * 0.6f, 1.0f);
  // fragColor = vec4(0.25f, 0.25f, 0.25f, 1.0f);

	// https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
	// tri planar texturing
	vec3 blending = abs(gs_out.normal);
	blending = normalize(max(blending, 0.00001f)); // force values from 0 to 1
	float b = blending.x + blending.y + blending.z;
	blending /= vec3(b, b, b);

	float scale = 1.0f;
	vec4 xColor = texture(rockTextureX, gs_out.worldSpaceCoordinates.yz * scale);
	vec4 yColor = texture(rockTextureY, gs_out.worldSpaceCoordinates.xz * scale);
	vec4 zColor = texture(rockTextureZ, gs_out.worldSpaceCoordinates.xy * scale);

	fragColor = xColor * blending.x + yColor * blending.y + zColor * blending.z;
}
