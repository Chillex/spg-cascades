#version 330 core

layout(location = 0) out vec3 color;

in VS_OUT {
  vec2 position;
} fs_in;

const vec2 pillars[3] = vec2[3]
  (
    vec2(0.5f, 0.1f),
    vec2(-0.2f, 0.6f),
    vec2(-0.4f, -0.6f)
  );

float Pillars(float f, float size)
{
  for(int p = 0; p < pillars.length(); ++p)
  {
    f += 1 / length(fs_in.position.xy - pillars[p].xy) - size;
  }

  return f;
}

float NegativeCenter(float f, float size)
{
  f -= (1 / length(fs_in.position.xy) - size);
  return f;
}

float NegativeEdge(float f, float size)
{
  f = f - pow(length(fs_in.position.xy), size);

  return f;
}

float Helix(float f, float size)
{
  // TODO: values need to be replaced with layer
  vec2 vec = vec2(cos(0.0f), sin(0.0f));
  f += dot(vec, fs_in.position.xy) * size;

  return f;
}

float CalculateDensity()
{
  float f = 0.0f;
  float size = 3.0f;

  // Add Pillars
  f = Pillars(f, size);

  // Add Negative Center
  f = NegativeCenter(f, size);

  // Add Negative outer edge
  f = NegativeEdge(f, 4 * size);

  // Add Helix
  f = Helix(f, size);

  return f;
}

void main()
{
  float f = CalculateDensity();

  color = vec3(f, 0, 0);
}
