#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 UVs;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out VS_OUT {
  vec3 fragmentPosition;
  vec2 UVs;
  vec3 tangentLightPosition;
  vec3 tangentViewPosition;
  vec3 tangentFragmentPosition;
  vec3 vertNormal;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
  gl_Position = projection * view * model * vec4(position, 1.0f);
  vs_out.fragmentPosition = vec3(model * vec4(position, 1.0f));
  vs_out.UVs = UVs;

  mat3 normalMatrix = transpose(inverse(mat3(model)));
  vec3 T = normalize(normalMatrix * tangent);
  vec3 B = normalize(normalMatrix * bitangent);
  vec3 N = normalize(normalMatrix * normal);
  mat3 TBN = transpose(mat3(T, B, N));

  vs_out.tangentLightPosition = TBN * lightPos;
  vs_out.tangentViewPosition = TBN * viewPos;
  vs_out.tangentFragmentPosition = TBN * vs_out.fragmentPosition;
}
