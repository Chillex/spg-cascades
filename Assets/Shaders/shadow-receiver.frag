#version 330

in VS_OUT {
  vec4 normalEye;
  vec4 positionEye;
  vec4 shadowCaster;
  vec2 texCoords;
} fs_in;

out vec4 fragColor;

uniform sampler2D shadowMap;

uniform vec3 objectColor;

// From http://fabiensanglard.net/shadowmappingVSM/index.php
float chebyshevUpperBound(vec2 coords, float distance)
{
    // we retrive depth and depth*depth
		vec2 moments = texture2D(shadowMap, coords).rg;

    // fragment is before light occluder
		if (distance <= moments.x)
			return 1.0f;

    // the fragment is either in shadow or penumbra
    // we now use chebyshev's upperBound to check how likely this fragment is to be lit
		float variance = moments.y - (moments.x * moments.x);
		variance = max(variance, 0.000002f);

		float dist = distance - moments.x;
		float pMax = variance / (variance + dist * dist);

    // reduce light bleeding
    // from https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch08.html
    pMax = smoothstep(0.1f, 1.0f, pMax);

    return pMax;
}

void main()
{
  vec3 fragment = vec3(fs_in.positionEye);
  vec3 normal = vec3(fs_in.normalEye);
  vec3 viewDir = normalize(-fragment);

  // bring shadows to [0-1] space
  vec4 shadowCasterNorm = fs_in.shadowCaster / fs_in.shadowCaster.w;
  shadowCasterNorm = shadowCasterNorm * 0.5f + 0.5f;

  float shadowFactor = 1.0f; // no shadow

  // check if we are outside of the shadow map
  bool isOutsideShadowMap = fs_in.shadowCaster.w <= 0.0f || (shadowCasterNorm.x < 0.0f || shadowCasterNorm.y < 0.0f) || (shadowCasterNorm.x >= 1.0f || shadowCasterNorm.y >= 1.0f);
  if (!isOutsideShadowMap)
  {
    // if we are inside the shadow map, get the shadow amount with chebyshev
    shadowFactor = chebyshevUpperBound(shadowCasterNorm.xy, shadowCasterNorm.z);
  }

  fragColor = vec4(objectColor * shadowFactor, 1.0f);
}
