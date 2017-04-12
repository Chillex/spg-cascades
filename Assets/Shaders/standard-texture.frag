#version 330 core
in VS_OUT {
	vec3 color;
	vec2 texCoords;
} vs_out;

out vec4 color;

uniform sampler2D textureSampler;

void main()
{
	color = texture(textureSampler, vs_out.texCoords) * vec4(vs_out.color, 1.0f);
}
