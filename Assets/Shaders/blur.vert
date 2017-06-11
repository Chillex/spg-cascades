#version 330

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texCoords;

out vec2 vTexCoords;

void main()
{
		gl_Position = vec4(position, 1.0);
		vTexCoords = texCoords;
};
