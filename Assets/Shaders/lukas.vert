#version 410

layout (location = 0) in vec2 uv;

out VertexData
{
	vec4 position;
	vec4 color;
	vec3 normal;
	int mc_case;
	vec4 field0123;
	vec4 field4567;
	vec3 cube_coords[8];
} vertex_data;

uniform sampler3D density_texture;

layout (shared) uniform yPos {
	float y_pos_slice[256];
};

vec3 calculateNormal(vec3 uvw, sampler3D tex, vec4 step_s)
{
	vec3 gradient = vec3(
		texture(tex, uvw + step_s.xww).x - texture(tex, uvw - step_s.xww).x,
		texture(tex, uvw + step_s.wwy).x - texture(tex, uvw - step_s.wwy).x,
		texture(tex, uvw + step_s.wzw).x - texture(tex, uvw - step_s.wzw).x
	);

	return normalize(-gradient);
}

void main()
{
	int instance = gl_InstanceID;
	vec3 inv_voxelSize = vec3(1/31.0f, 1/31.0f, 1/31.0f);

	vec3 wsCoord;
	wsCoord.xz = (uv.xy + 1.0f) / 2.0f;
	// // TODO: Sample y position from uniform buffer
	wsCoord.y  = y_pos_slice[instance]; // - (0.5f * inv_voxelSize.y); //* 0.5f;

	vec4 step = vec4(inv_voxelSize, 0.0f);

	vertex_data.position = vec4(wsCoord, 1.0f);

	vec4 field0123;
	vec4 field4567;
	int cube_case = 0;

	vertex_data.cube_coords[0] = wsCoord.xzy + step.www;
	field0123.x = texture(density_texture, vertex_data.cube_coords[0]).x;
	vertex_data.cube_coords[1] = wsCoord.xzy + step.wwy;
	field0123.y = texture(density_texture, vertex_data.cube_coords[1]).x;
	vertex_data.cube_coords[2] = wsCoord.xzy + step.xwy;
	field0123.z = texture(density_texture, vertex_data.cube_coords[2]).x;
	vertex_data.cube_coords[3] = wsCoord.xzy + step.xww;
	field0123.w = texture(density_texture, vertex_data.cube_coords[3]).x;

	vertex_data.cube_coords[4] = wsCoord.xzy + step.wzw;
	field4567.x = texture(density_texture, vertex_data.cube_coords[4]).x;
	vertex_data.cube_coords[5] = wsCoord.xzy + step.wzy;
	field4567.y = texture(density_texture, vertex_data.cube_coords[5]).x;
	vertex_data.cube_coords[6] = wsCoord.xzy + step.xzy;
	field4567.z = texture(density_texture, vertex_data.cube_coords[6]).x;
	vertex_data.cube_coords[7] = wsCoord.xzy + step.xzw;
	field4567.w = texture(density_texture, vertex_data.cube_coords[7]).x;

	cube_case |= int(field0123.x < 0.0f) << 0;
	cube_case |= int(field0123.y < 0.0f) << 1;
	cube_case |= int(field0123.z < 0.0f) << 2;
	cube_case |= int(field0123.w < 0.0f) << 3;

	cube_case |= int(field4567.x < 0.0f) << 4;
	cube_case |= int(field4567.y < 0.0f) << 5;
	cube_case |= int(field4567.z < 0.0f) << 6;
	cube_case |= int(field4567.w < 0.0f) << 7;

	if (cube_case == 0 || cube_case == 255)
	{
		vertex_data.color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		vertex_data.color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	vertex_data.mc_case = cube_case;
	vertex_data.field0123 = field0123;
	vertex_data.field4567 = field4567;
	vertex_data.normal = calculateNormal(wsCoord.xzy, density_texture, step);
}
