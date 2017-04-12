#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 color;
} gs_in[];

out vec3 fColor;

void build_house(vec4 position)
{
  float dimensions = 0.15f;
  fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
  gl_Position = position + vec4(-dimensions, -dimensions * 1.75f, 0.0f, 0.0f);
  EmitVertex();
  gl_Position = position + vec4( dimensions, -dimensions * 1.75f, 0.0f, 0.0f);
  EmitVertex();
  gl_Position = position + vec4(-dimensions,  dimensions, 0.0f, 0.0f);
  EmitVertex();
  gl_Position = position + vec4( dimensions,  dimensions, 0.0f, 0.0f);
  EmitVertex();
  gl_Position = position + vec4( 0.0f, dimensions * 2.5f, 0.0f, 0.0f);
  EmitVertex();
  EndPrimitive();
}

void main()
{
  build_house(gl_in[0].gl_Position);
}
