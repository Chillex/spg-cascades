#version 410 core

layout (points) in;
layout (points, max_vertices = 32) out;

in VS_OUT {
  vec3 position;
  vec3 velocity;
  float lifetime;
  int type;
} geo_in[];

out vec3 outPosition;
out vec3 outVelocity;
out float outLifetime;
out int outType;

uniform float deltaTime;
uniform float emmiterRate;

uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

float random( float f ) {
    const uint mantissaMask = 0x007FFFFFu;
    const uint one          = 0x3F800000u;

    uint h = hash( floatBitsToUint( f ) );
    h &= mantissaMask;
    h |= one;

    float  r2 = uintBitsToFloat( h );
    return r2 - 1.0;
}

void HandleEmitter()
{
  float newLifetime = geo_in[0].lifetime - deltaTime;

  // ouput the emmiter particle again
  outPosition = geo_in[0].position;
  outVelocity = geo_in[0].velocity;
  outLifetime = (newLifetime <= 0.0f) ? emmiterRate : newLifetime;
  outType = geo_in[0].type;

  EmitVertex();
  EndPrimitive();

  // Spawn new fuse particle
  if(newLifetime <= 0.0f)
  {
    vec3 randomVelocity = vec3(
      0.5f - random(deltaTime),
      2.0f + random(deltaTime*deltaTime),
      0.5f - random(deltaTime*deltaTime*deltaTime)
    );
    // also generate a new particle
    outPosition = geo_in[0].position;
    outVelocity = randomVelocity;
    outLifetime = 2.0f + random(deltaTime*deltaTime*deltaTime*deltaTime);
    outType = 1;

    EmitVertex();
    EndPrimitive();
  }
}

void SpawnExplosionParticles(vec3 position)
{
  for (int i = 0; i < 32; ++i)
  {
    vec3 randomVelocity = vec3(
      1.0f - 2.0f * random(deltaTime*i),
      1.0f - 2.0f * random(deltaTime*deltaTime*i),
      1.0f - 2.0f * random(deltaTime*deltaTime*deltaTime*i)
    );
    outPosition = position;
    outVelocity = randomVelocity;
    outLifetime = random(deltaTime*deltaTime*deltaTime*deltaTime*i);
    outType = 2;

    EmitVertex();
    EndPrimitive();
  }
}

void HandleFuse()
{
  float newLifetime = geo_in[0].lifetime - deltaTime;

  if(newLifetime <= 0.0f)
  {
    SpawnExplosionParticles(geo_in[0].position);
    return;
  }

  // move the particle
  outPosition = geo_in[0].position + (geo_in[0].velocity * deltaTime);
  outVelocity = geo_in[0].velocity;
  outLifetime = newLifetime;
  outType = geo_in[0].type;

  EmitVertex();
  EndPrimitive();
}

void HandleExplosion()
{
  float newLifetime = geo_in[0].lifetime - deltaTime;

  if(newLifetime <= 0.0f)
    return;

  // move the particle
  outPosition = geo_in[0].position + (geo_in[0].velocity * deltaTime);
  outVelocity = geo_in[0].velocity;
  outLifetime = newLifetime;
  outType = geo_in[0].type;

  EmitVertex();
  EndPrimitive();
}

void main() {
  if(geo_in[0].type == 0) // emmiter particle
  {
    HandleEmitter();
  }
  else if(geo_in[0].type == 1)
  {
    HandleFuse();
  }
  else if(geo_in[0].type == 2)
  {
    HandleExplosion();
  }
}
