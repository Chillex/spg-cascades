#version 430

in GS_OUT {
	vec3 normal;
} gs_out;

out vec4 fragColor;

void main()
{
  fragColor = vec4(gs_out.normal * 0.6f, 1.0f);
  // fragColor = vec4(0.25f, 0.25f, 0.25f, 1.0f);
}
