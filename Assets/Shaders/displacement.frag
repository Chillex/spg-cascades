#version 330 core
out vec4 fragColor;

in VS_OUT {
  vec3 fragmentPosition;
  vec2 UVs;
  vec3 tangentLightPosition;
  vec3 tangentViewPosition;
  vec3 tangentFragmentPosition;
  vec3 vertNormal;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D displacementMap;

uniform bool useDisplacement;
uniform int displacementInitialSteps;
uniform int displacementRefinementSteps;

vec2 Displacement(vec2 uv, vec3 eyeVec, sampler2D displacementTex, int initialSteps, int refinementSteps)
{
  // calculate step sizes
	float stepSize = 1.0f / float(initialSteps);
	float refinementStepSize = 1.0f / float(refinementSteps);

  // calculate uv delta and set starting height
	vec2 newUVs = uv;
	vec2 deltaUV = -eyeVec.xy * 2.5f * 0.035f / float(initialSteps);
	float searchHeight = 1.0f;

  // start sampling with initial steps
	float prevHits = 0.0f;
	float hitHeight = 0.0f;

	for (int i = 0; i < initialSteps; ++i)
	{
    // set height and uvs for this step
		searchHeight -= stepSize;
		newUVs += deltaUV;

    // get height on this step from map and check if we hit
		float currentHeight = texture(displacementTex, newUVs).x;
		float isFirstHit = clamp((currentHeight - searchHeight - prevHits) * 4999999.0f, 0.0f, 1.0f);

    // save the hit height if we hit
		hitHeight += isFirstHit * searchHeight;
		prevHits += isFirstHit;
	}

  // calculate UVs after inital steps
	newUVs = uv + deltaUV * (1.0f - hitHeight) * initialSteps - deltaUV;

  // calculations before starting the refinement
	vec2 uvsBeforeRefinement = newUVs;
	searchHeight = hitHeight + stepSize - 0.02f; // extra offset helpts reduce grey artifacts
	float heightBeforeRefinement = searchHeight;

  // scale delta UV with refinement size
	deltaUV *= refinementStepSize;
  // start sampling with refinement steps
	prevHits = 0.0f;
	hitHeight = 0.0f;

	for (int i = 0; i < refinementSteps; ++i)
	{
    // set height and uvs for this step
		searchHeight -= stepSize * refinementStepSize;
		newUVs += deltaUV;

    // get height on this step from map and check if we hit
		float currentHeight = texture(displacementTex, newUVs).x;
		float isFirstHit = clamp((currentHeight - searchHeight - prevHits) * 4999999.0f, 0.0f, 1.0f);

    // save the hit height if we hit
		hitHeight += isFirstHit * searchHeight;
		prevHits += isFirstHit;
	}

  // calculate final UVs
	newUVs = uvsBeforeRefinement + deltaUV * (heightBeforeRefinement - hitHeight) * initialSteps * refinementSteps;
	return newUVs;
}

void main()
{
  vec3 viewDir = normalize(fs_in.tangentViewPosition - fs_in.tangentFragmentPosition);
  vec2 UVs = fs_in.UVs;

  if(useDisplacement)
    UVs = Displacement(UVs, viewDir, displacementMap, displacementInitialSteps, displacementRefinementSteps);

  // get normal from texture
  vec3 normal = texture(normalMap, UVs).rgb;
  normal = normalize(normal * 2.0f - 1.0f);

  // get color
  vec3 color = texture(diffuseMap, UVs).rgb;

  // ambient
  vec3 ambient = 0.1f * color;

  // diffuse
  vec3 lightDir = normalize(fs_in.tangentLightPosition - fs_in.tangentFragmentPosition);
  float diff = max(dot(lightDir, normal), 0.0f);
  vec3 diffuse = diff * color;

  // specular
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
  vec3 specular = spec * color;

  // fragColor = vec4(specular, 1.0f);
  fragColor = vec4(ambient + diffuse + specular, 1.0f);
}
