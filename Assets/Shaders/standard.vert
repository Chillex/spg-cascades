#version 330 core
layout (location = 0) in vec3 position;

out vec3 vColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	gl_PointSize = 10.0f;
	vColor = objectColor;
}
