#version 430

in GS_OUT {
	vec3 normal;
  // vec3 tangent;
	// vec3 bitangent;
  vec3 worldSpaceCoordinates;
	vec3 cameraPosition;
} gs_out;

out vec4 fragColor;

uniform sampler2D rockTextureX;
uniform sampler2D rockTextureY;
uniform sampler2D rockTextureZ;

uniform vec3 lightPos;

void main()
{
  // fragColor = vec4(gs_out.normal * 0.6f, 1.0f);
  // fragColor = vec4(0.25f, 0.25f, 0.25f, 1.0f);

	// https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
	// tri planar texturing + displacement mapping
	vec3 blending = abs(gs_out.normal);
	blending = normalize(max(blending, 0.00001f)); // force values to sum to 1.0
	float b = blending.x + blending.y + blending.z;
	blending /= vec3(b, b, b);

	float scale = 5.0f;
	vec2 uv1 = gs_out.worldSpaceCoordinates.yz;// * scale;
	vec2 uv2 = gs_out.worldSpaceCoordinates.xz;// * scale;
	vec2 uv3 = gs_out.worldSpaceCoordinates.xy;// * scale;

	vec3 normal = gs_out.normal;

	vec4 xColor = texture(rockTextureX, uv1);
	vec4 yColor = texture(rockTextureY, uv2);
	vec4 zColor = texture(rockTextureZ, uv3);

	vec4 color = xColor * blending.x + yColor * blending.y + zColor * blending.z;

	// lighting
	float ambient = 0.1f;

	vec3 lightDir = normalize(lightPos - gs_out.worldSpaceCoordinates);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	vec4 lightingResult = (ambient + diffuse) * color;
	fragColor = lightingResult;
}
