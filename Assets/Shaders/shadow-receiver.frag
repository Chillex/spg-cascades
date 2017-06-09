#version 330

in VS_OUT {
  vec4 normalEye;
  vec4 positionEye;
  vec4 shadowCaster;
  vec2 texCoords;
} fs_in;

out vec4 fragColor;

uniform sampler2D shadowMap;

uniform mat4 view;
uniform vec3 lightPos;

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
		variance = max(variance, 0.00002);

		float dist = distance - moments.x;
		return variance / (variance + dist * dist);
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

  // LIGHTING
  vec3 light = vec3(view * vec4(lightPos, 1.0f));

  vec4 diffColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

  vec3 positionToLight = light - fragment;
  vec3 lightDir = normalize(positionToLight);

  // calculate angle between normal and light
  float angle = clamp(dot(lightDir, normal), 0.0f, 1.0f);

  float constAtt = 1.0f;
  float linearAtt = 0.0f;
  float quadAtt = 0.0f;

  float attenuation = 1.0f / (constAtt + linearAtt * length(positionToLight) + quadAtt * pow(length(positionToLight), 2));

  vec4 lightColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
  vec4 diffuse = diffColor * lightColor * angle * attenuation;

  vec4 result;
  result += vec4(0.1f, 0.1f, 0.1f, 1.0f) * diffColor; // ambient
  result += diffuse * shadowFactor; // diffuse

  fragColor = vec4(vec3(result), 1.0f);
}
