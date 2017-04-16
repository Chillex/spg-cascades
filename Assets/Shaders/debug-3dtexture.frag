#version 330 core
in VS_OUT {
	vec3 color;
	vec3 texCoords;
} vs_out;

out vec4 color;

uniform sampler3D textureSampler;

void main()
{
	vec4 textureColor = texture(textureSampler, vs_out.texCoords);
	vec4 vertexColor = vec4(vs_out.color, 1.0f);

	color = textureColor * vertexColor;
}
