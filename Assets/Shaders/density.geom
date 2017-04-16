#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
  vec2 position;
  int instanceID;
} geo_in[];

out GEO_OUT {
  vec3 position;
} geo_out;

void main() {
  for(int i = 0; i < 3; ++i)
  {
    gl_Position = gl_in[i].gl_Position;
    gl_Layer = geo_in[i].instanceID;
    geo_out.position = vec3(geo_in[i].position, gl_Layer);

    EmitVertex();
  }

  EndPrimitive();
}
