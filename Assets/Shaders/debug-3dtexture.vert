#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
	vec3 color;
  vec3 texCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float textureZCoord;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);

	vs_out.color = color;
	vs_out.texCoords = vec3(texCoords, textureZCoord);
}
