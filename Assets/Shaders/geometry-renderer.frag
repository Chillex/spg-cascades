#version 430

in VS_OUT {
	vec3 position;
  vec3 normal;
} fs_in;

out vec4 fragColor;

uniform sampler2D rockTextureX;
uniform sampler2D rockTextureY;
uniform sampler2D rockTextureZ;

uniform vec3 cameraPos;
uniform vec3 lightPos;

void main() {
	// https://gamedevelopment.tutsplus.com/articles/use-tri-planar-texture-mapping-for-better-terrain--gamedev-13821
	// tri planar texturing + displacement mapping
	vec3 blending = abs(fs_in.normal);
	blending = normalize(max(blending, 0.00001f)); // force values to sum to 1.0
	float b = blending.x + blending.y + blending.z;
	blending /= vec3(b, b, b);

	float scale = 3.0f;
	vec2 uv1 = fs_in.position.yz * scale;
	vec2 uv2 = fs_in.position.xz * scale;
	vec2 uv3 = fs_in.position.xy * scale;

	vec3 normal = fs_in.normal;

	vec4 xColor = texture(rockTextureX, uv1);
	vec4 yColor = texture(rockTextureY, uv2);
	vec4 zColor = texture(rockTextureZ, uv3);

	vec4 color = xColor * blending.x + yColor * blending.y + zColor * blending.z;

	// lighting
	float ambient = 0.1f;

	vec3 lightDir = normalize(lightPos - fs_in.position);
	float diffuse = max(dot(normal, lightDir), 0.0f);

	vec4 lightingResult = (ambient + diffuse) * color;
	fragColor = lightingResult;
}
