#version 330 core

layout(location = 0) out vec3 color; // GL_COLOR_ATACHMENT0 - GL_RGB

in GEO_OUT {
  vec3 position;
} fs_in;

const vec2 pillars[3] = vec2[3]
  (
    vec2(0.5f, 0.1f),
    vec2(-0.2f, 0.6f),
    vec2(-0.4f, -0.6f)
  );

float Pillars()
{
  float f = 0.0f;

  for(int p = 0; p < pillars.length(); ++p)
  {
    f += 1 / (length(fs_in.position.xy - pillars[p].xy) * 2.0f) - 1;
  }

  return f;
}

float NegativeCenter()
{
  return (1 / length(fs_in.position.xy) - 1);
}

float NegativeEdge()
{
  return pow(length(fs_in.position.xy), 1);
}

float Helix()
{
  vec2 vec = vec2(cos(fs_in.position.z / 3.5f), sin(fs_in.position.z / 3.5f));
  return dot(vec, fs_in.position.xy) * 0.5f;
}

float Shelves()
{
  return cos(fs_in.position.z / 12.0f) * 1.5f;
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


float CalculateDensity()
{
  float f = 0.0f;

  // Add Pillars
  f += Pillars();

  // Add Negative Center
  f -= NegativeCenter();

  // Add Negative outer edge
  f -= NegativeEdge();

  // Add Helix
  f += Helix();

  // Add Shelves
  f += Shelves();

  // Add Noise
  //f += rand(fs_in.position.xy);

  return f;
}

void main()
{
  float f = CalculateDensity();

  color = vec3(f, 0, 0);
}
