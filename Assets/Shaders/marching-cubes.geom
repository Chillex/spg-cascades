#version 430

layout (points) in;
layout (triangle_strip, max_vertices = 15) out;

layout (shared) uniform triangleConnectionLUT {
  int caseToTriangles[4096]; // 256 * 16
};

const vec3[8] vertexOffsetLUT = vec3[8](
    vec3(0.0, 0.0, 0.0), vec3(1.0, 0.0, 0.0), vec3(1.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0), vec3(1.0, 0.0, 1.0), vec3(1.0, 1.0, 1.0), vec3(0.0, 1.0, 1.0)
  );

const uvec2[12] edgeConnectionLUT = uvec2[12](
    uvec2(0, 1), uvec2(1, 2), uvec2(2, 3), uvec2(3, 0),
    uvec2(4, 5), uvec2(5, 6), uvec2(6, 7), uvec2(7, 4),
    uvec2(0, 4), uvec2(1, 5), uvec2(2, 6), uvec2(3, 7)
  );

const vec3[12] edgeDirectionLUT = vec3[12](
    vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0),
    vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(-1.0, 0.0, 0.0), vec3(0.0, -1.0, 0.0),
    vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, 1.0), vec3( 0.0, 0.0, 1.0), vec3(0.0,  0.0, 1.0)
  );

in VS_OUT {
  vec3 worldSpaceCoordinates;
  uint marchingCubesCase;
	vec3 cornerCoordinates[8];
  float densities[8];
} geo_in[];

out GS_OUT {
	vec3 normal;
} geo_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 GetNormal(vec3 p1, vec3 p2, vec3 p3)
{
	vec3 normal;

	vec3 u = p2 - p1;
	vec3 v = p3 - p1;

	normal.x = (u.y * v.z) - (u.z * v.y);
	normal.y = (u.z * v.x) - (u.x * v.z);
	normal.z = (u.x * v.y) - (u.y * v.x);

	return normalize(normal);
}

vec3 InterpolateVertex(float isoLevel, vec3 vertex1, float density1, vec3 vertex2, float density2)
{
  float interpolationValue = (isoLevel - density1) / (density2 - density1);
  return mix(vertex1, vertex2, interpolationValue);
}

vec3 GetVertexPositionOnEdge(int edgeIndex, vec3 vertexCoords[8], float density[8])
{
  uvec2 verticesOnEdge = edgeConnectionLUT[edgeIndex];
  return InterpolateVertex(0.0f, vertexCoords[verticesOnEdge.x], density[verticesOnEdge.x], vertexCoords[verticesOnEdge.y], density[verticesOnEdge.y]);
}

void main() {
  uint lutPosition = geo_in[0].marchingCubesCase * 16; // 16 ints per marching cubes case

  for(int i = 0; caseToTriangles[lutPosition + i] != -1; i += 3)
  {
    vec3 p1 = GetVertexPositionOnEdge(caseToTriangles[lutPosition + i], geo_in[0].cornerCoordinates, geo_in[0].densities).xzy;
		vec3 p2 = GetVertexPositionOnEdge(caseToTriangles[lutPosition + i + 1], geo_in[0].cornerCoordinates, geo_in[0].densities).xzy;
		vec3 p3 = GetVertexPositionOnEdge(caseToTriangles[lutPosition + i + 2], geo_in[0].cornerCoordinates, geo_in[0].densities).xzy;
		vec3 normal = GetNormal(p1, p2, p3);

		geo_out.normal = normal;
		gl_Position = projection * view * model * vec4(p1, 1.0f);
    EmitVertex();

		geo_out.normal = normal;
    gl_Position = projection * view * model * vec4(p2, 1.0f);
    EmitVertex();

		geo_out.normal = normal;
    gl_Position = projection * view * model * vec4(p3, 1.0f);
    EmitVertex();

    EndPrimitive();
  }
}
