#version 430

layout (location = 0) in vec2 position;

out VS_OUT {
  vec3 worldSpaceCoordinates;
  uint marchingCubesCase;
  vec3 cornerCoordinates[8];
  float densities[8];
} vs_out;

layout (shared) uniform yPosition {
	float yPos[512];
};

// handy variables for calculation of voxels
uniform vec3 invertedVoxelDimension;

uniform sampler3D densityTexture;

void main()
{
  vs_out.worldSpaceCoordinates.xz = position.xy;
  vs_out.worldSpaceCoordinates.y = yPos[gl_InstanceID];

  //sample texture to get density at all 8 corners of the voxel
  vec4 sampleStep = vec4(invertedVoxelDimension, 0.0f);

  vs_out.cornerCoordinates[0] = vs_out.worldSpaceCoordinates.xzy + sampleStep.www;
  vs_out.densities[0] = texture(densityTexture, vs_out.cornerCoordinates[0]).x;
  vs_out.cornerCoordinates[1] = vs_out.worldSpaceCoordinates.xzy + sampleStep.wwy;
  vs_out.densities[1] = texture(densityTexture, vs_out.cornerCoordinates[1]).x;
  vs_out.cornerCoordinates[2] = vs_out.worldSpaceCoordinates.xzy + sampleStep.xwy;
  vs_out.densities[2] = texture(densityTexture, vs_out.cornerCoordinates[2]).x;
  vs_out.cornerCoordinates[3] = vs_out.worldSpaceCoordinates.xzy + sampleStep.xww;
  vs_out.densities[3] = texture(densityTexture, vs_out.cornerCoordinates[3]).x;

  vs_out.cornerCoordinates[4] = vs_out.worldSpaceCoordinates.xzy + sampleStep.wzw;
  vs_out.densities[4] = texture(densityTexture, vs_out.cornerCoordinates[4]).x;
  vs_out.cornerCoordinates[5] = vs_out.worldSpaceCoordinates.xzy + sampleStep.wzy;
  vs_out.densities[5] = texture(densityTexture, vs_out.cornerCoordinates[5]).x;
  vs_out.cornerCoordinates[6] = vs_out.worldSpaceCoordinates.xzy + sampleStep.xzy;
  vs_out.densities[6] = texture(densityTexture, vs_out.cornerCoordinates[6]).x;
  vs_out.cornerCoordinates[7] = vs_out.worldSpaceCoordinates.xzy + sampleStep.xzw;
  vs_out.densities[7] = texture(densityTexture, vs_out.cornerCoordinates[7]).x;

  uvec4 n0123 = uvec4(clamp(vec4(vs_out.densities[0], vs_out.densities[1], vs_out.densities[2], vs_out.densities[3]) * 99999, 0.0f, 1.0f));
  uvec4 n4567 = uvec4(clamp(vec4(vs_out.densities[4], vs_out.densities[5], vs_out.densities[6], vs_out.densities[7]) * 99999, 0.0f, 1.0f));

  vs_out.marchingCubesCase =  (n0123.x     ) | (n4567.x << 4)
                            | (n0123.y << 1) | (n4567.y << 5)
                            | (n0123.z << 2) | (n4567.z << 6)
                            | (n0123.w << 3) | (n4567.w << 7);
}
