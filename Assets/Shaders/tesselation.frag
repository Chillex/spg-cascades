#version 430 core

in TES_OUT
{
  vec3 position;
  vec3 normal;
  vec2 texCoords;
} fs_in;

out vec4 fragColor;

void main()
{
  fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}
