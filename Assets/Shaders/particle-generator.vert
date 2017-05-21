#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;
layout (location = 2) in float lifetime;
layout (location = 3) in int type;

out VS_OUT {
  vec3 position;
  vec3 velocity;
  float lifetime;
  int type;
} vs_out;

void main()
{
  vs_out.position = position;
  vs_out.velocity = velocity;
  vs_out.lifetime = lifetime;
  vs_out.type = type;
}
