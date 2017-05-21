#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out VS_OUT {
  vec3 position;
  vec3 normal;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  vs_out.position = position;
  vs_out.normal = normal;

  gl_Position = projection * view * model * vec4(position, 1.0f);
}
