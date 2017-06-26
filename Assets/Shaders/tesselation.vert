#version 430 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 model;

out VS_OUT {
  vec3 position;
  vec3 normal;
  vec2 texCoords;
} vs_out;

void main()
{
  vs_out.position = (model * vec4(position, 1.0f)).xyz;
  vs_out.normal = (model * vec4(normal, 0.0f)).xyz;
  vs_out.texCoords = texCoords;
}
