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
uniform sampler2D rockTextureDisplacementX;
uniform sampler2D rockTextureDisplacementY;
uniform sampler2D rockTextureDisplacementZ;

uniform vec3 lightPos;

uniform bool useDisplacement;
uniform int displacementInitialSteps;
uniform int displacementRefinementSteps;

vec2 Displacement(vec2 uv, vec2 eyeVec, sampler2D displacementTex, int initialSteps, int refinementSteps)
{
	float stepSize = 1.0f / float(initialSteps);
	float refinementStepSize = 1.0f / float(refinementSteps);

	vec2 newUVs = uv;
	vec2 dUV = -eyeVec.xy * 0.1f / float(initialSteps);
	float searchHeight = 1.0f;

	float prevHits = 0.0f;
	float hitH = 0.0f;

	for (int i = 0; i < initialSteps; ++i)
	{
		searchHeight -= stepSize;
		newUVs += dUV;

		float currentHeight = texture(displacementTex, newUVs).x;
		float isFirstHit = clamp((currentHeight - searchHeight - prevHits) * 4999999.0f, 0.0f, 1.0f);

		hitH += isFirstHit * searchHeight;
		prevHits += isFirstHit;
	}

	newUVs = uv + dUV * (1.0f - hitH) * initialSteps - dUV;

	vec2 tmp = newUVs;
	searchHeight = hitH + stepSize;
	float start = searchHeight;

	dUV *= refinementStepSize;
	prevHits = 0.0f;
	hitH = 0.0f;

	for (int i = 0; i < refinementSteps; ++i)
	{
		searchHeight -= stepSize * refinementStepSize;
		newUVs += dUV;

		float currentHeight = texture(displacementTex, newUVs).x;
		float isFirstHit = clamp((currentHeight - searchHeight - prevHits) * 4999999.0f, 0.0f, 1.0f);

		hitH += isFirstHit * searchHeight;
		prevHits += isFirstHit;
	}

	newUVs = tmp + dUV * (start - hitH) * initialSteps * refinementSteps;
	return newUVs;
}

void main()
{
  // fragColor = vec4(gs_out.normal * 0.6f, 1.0f);
  // fragColor = vec4(0.25f, 0.25f, 0.25f, 1.0f);

	vec3 worldEyeVector = normalize(gs_out.cameraPosition - gs_out.worldSpaceCoordinates);
	vec2 tsEyeVec; // tangent space eye vector

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

	// https://www.opengl.org/discussion_boards/showthread.php/162857-Computing-the-tangent-space-in-the-fragment-shader
	// TODO: the vertex shader just transforms light, position and normal to camera space, so I left it out
	if(useDisplacement)
	{
		vec3 Q1 = dFdx(gs_out.worldSpaceCoordinates);
		vec3 Q2 = dFdy(gs_out.worldSpaceCoordinates);
		vec2 st1, st2;
		vec3 T, B;

		// X-Plane
		st1 = dFdx(uv1);
		st2 = dFdy(uv1);
		T = normalize(Q1 * st2.t - Q2 * st1.t);
		B = normalize(-Q1 * st2.s + Q2 * st1.s);
		mat3 TBN = mat3(T, B, gs_out.normal);
		// normal = gs_out.normal * TBN;
		tsEyeVec.x = dot(worldEyeVector, T);
		tsEyeVec.y = dot(worldEyeVector, B);
		uv1 = Displacement(uv1, tsEyeVec, rockTextureDisplacementX, displacementInitialSteps, displacementRefinementSteps);

		// Y-Plane
		st1 = dFdx(uv2);
		st2 = dFdy(uv2);
		T = normalize(Q1 * st2.t - Q2 * st1.t);
		B = normalize(-Q1 * st2.s + Q2 * st1.s);
		tsEyeVec.x = dot(worldEyeVector, T);
		tsEyeVec.y = dot(worldEyeVector, B);
		uv2 = Displacement(uv2, tsEyeVec, rockTextureDisplacementY, displacementInitialSteps, displacementRefinementSteps);

		// Z-Plane
		st1 = dFdx(uv3);
		st2 = dFdy(uv3);
		T = normalize(Q1 * st2.t - Q2 * st1.t);
		B = normalize(-Q1 * st2.s + Q2 * st1.s);
		tsEyeVec.x = dot(worldEyeVector, T);
		tsEyeVec.y = dot(worldEyeVector, B);
		uv3 = Displacement(uv3, tsEyeVec, rockTextureDisplacementZ, displacementInitialSteps, displacementRefinementSteps);
	}

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
