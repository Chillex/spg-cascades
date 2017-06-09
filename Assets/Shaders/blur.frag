#version 330

in vec2 vTexCoords;

uniform sampler2D texture;
uniform vec2 scale;

out vec4 fragColor;

void main()
{
  vec4 color = vec4(0.0f);

  color += texture2D(texture, vTexCoords.st + vec2(-3.0 * scale.x, -3.0 * scale.y)) * 0.015625;
	color += texture2D(texture, vTexCoords.st + vec2(-2.0 * scale.x, -2.0 * scale.y)) * 0.09375;
	color += texture2D(texture, vTexCoords.st + vec2(-1.0 * scale.x, -1.0 * scale.y)) * 0.234375;
	color += texture2D(texture, vTexCoords.st + vec2(0.0 , 0.0)) * 0.3125;
	color += texture2D(texture, vTexCoords.st + vec2(1.0 * scale.x,  1.0 * scale.y)) * 0.234375;
	color += texture2D(texture, vTexCoords.st + vec2(2.0 * scale.x,  2.0 * scale.y)) * 0.09375;
	color += texture2D(texture, vTexCoords.st + vec2(3.0 * scale.x, -3.0 * scale.y)) * 0.015625;

  fragColor = vec4(color.xyz, 1.0f);
}
