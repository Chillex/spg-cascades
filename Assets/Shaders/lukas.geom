#version 410

layout(points) in;
layout(triangle_strip, max_vertices = 15) out;

in VertexData
{
	vec4 position;
	vec4 color;
	vec3 normal;
	int mc_case;
	vec4 field0123;
	vec4 field4567;
	vec3 cube_coords[8];
} vertex_data[];

layout(shared) uniform lut_poly {
	uint  case_to_poly[256];
};

layout(shared) uniform lut_tri {
	int case_to_triangles[4096];
};

out vec3 position;
out vec3 color;

void interpolateVertex(in float iso, in vec3 vec0, in float dens0, in vec3 vec1, in float dens1, inout vec3 vertex)
{
	float interpolation_value = (iso - dens0) / (dens1 - dens0);
	vertex = mix(vec0, vec1, interpolation_value);
}

void main()
{
	uint num_poly = case_to_poly[vertex_data[0].mc_case];
	uint table_pos = vertex_data[0].mc_case * 16;

	vec3 vertexList[12];

	interpolateVertex(0, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertexList[0]);
	interpolateVertex(0, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertexList[1]);
	interpolateVertex(0, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertexList[2]);
	interpolateVertex(0, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertexList[3]);

	interpolateVertex(0, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertexList[4]);
	interpolateVertex(0, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertexList[5]);
	interpolateVertex(0, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertexList[6]);
	interpolateVertex(0, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertexList[7]);

	interpolateVertex(0, vertex_data[0].cube_coords[0], vertex_data[0].field0123.x, vertex_data[0].cube_coords[4], vertex_data[0].field4567.x, vertexList[8]);
	interpolateVertex(0, vertex_data[0].cube_coords[1], vertex_data[0].field0123.y, vertex_data[0].cube_coords[5], vertex_data[0].field4567.y, vertexList[9]);
	interpolateVertex(0, vertex_data[0].cube_coords[2], vertex_data[0].field0123.z, vertex_data[0].cube_coords[6], vertex_data[0].field4567.z, vertexList[10]);
	interpolateVertex(0, vertex_data[0].cube_coords[3], vertex_data[0].field0123.w, vertex_data[0].cube_coords[7], vertex_data[0].field4567.w, vertexList[11]);

	for (uint i = 0; case_to_triangles[table_pos + i] != -1; i += 3)
	{
		position = vec3(vertexList[case_to_triangles[table_pos + i + 0]]).xzy;
		color = vertex_data[0].normal; //vec3(1.0f, 0.0f, 0.0f);
    gl_Position = vec4(position, 1.0f);
		EmitVertex();
		position = vec3(vertexList[case_to_triangles[table_pos + i + 1]]).xzy;
		color = vertex_data[0].normal; //vec3(1.0f, 0.0f, 0.0f);
    gl_Position = vec4(position, 1.0f);
		EmitVertex();
		position = vec3(vertexList[case_to_triangles[table_pos + i + 2]]).xzy;
		color = vertex_data[0].normal; //vec3(1.0f, 0.0f, 0.0f);
    gl_Position = vec4(position, 1.0f);
		EmitVertex();

		EndPrimitive();
	}
}
