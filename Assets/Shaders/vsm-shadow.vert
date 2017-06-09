#version 330 core

layout (location = 0) in vec3 position;

out vec4 vPosition;

uniform mat4 model;
uniform mat4 cameraToShadowProjection;

void main()
{
	gl_Position = cameraToShadowProjection * model * vec4(position, 1.0f);
	vPosition = gl_Position;
}
