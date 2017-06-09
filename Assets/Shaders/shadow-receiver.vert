#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 normal;

out VS_OUT {
	vec4 normalEye;
  vec4 positionEye;
  vec4 shadowCaster;
  vec2 texCoords;
} vs_out;

uniform mat4 cameraToShadowProjection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  vs_out.normalEye = view * model * vec4(normal, 0.0f);
  vs_out.positionEye = view * model * vec4(position, 1.0f);
  vs_out.shadowCaster = cameraToShadowProjection * model * vec4(position, 1.0f);
  vs_out.texCoords = texCoords;

  gl_Position = projection * view * model * vec4(position, 1.0f);
}
