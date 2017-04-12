#version 330 core

layout (location = 0) in vec2 position;

out VS_OUT {
  vec2 position;
} vs_out;

void main()
{
  vs_out.position = position;
  
  gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
}
