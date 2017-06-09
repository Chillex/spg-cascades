#version 330 core

in vec4 vPosition;
out vec4 fragColor;

// http://fabiensanglard.net/shadowmappingVSM/index.php
void main()
{
	float depth = vPosition.z / vPosition.w;
  depth = depth * 0.5f + 0.5f; // from [-1, 1] to [0, 1]

  float moment1 = depth;
  float moment2 = depth * depth;

  float dx = dFdx(depth);
  float dy = dFdy(depth);
  moment2 += 0.25f * (dx * dx + dy * dy);

  fragColor = vec4(moment1, moment2, 0.0f, 0.0f);
}
