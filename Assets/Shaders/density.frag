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

float Pillars(vec3 position)
{
  float f = 0.0f;

  float sinVal = sin(position.z * 0.01f);
  float cosVal = cos(position.z * 0.01f);

  for(int p = 0; p < pillars.length; ++p)
  {
    vec2 pillar = pillars[p];
    pillar.x *= sinVal;
    pillar.y *= cosVal;

    f += 1 / (length(position.xy - pillar.xy) * 2.0f) - 1;
  }

  return f;
}

float NegativeCenter(vec3 position)
{
  return (1 / length(position.xy) - 1);
}

float NegativeEdge(vec3 position)
{
  return pow(length(position.xy), 1) * 1.1f;
}

float Helix(vec3 position)
{
  vec2 vec = vec2(cos(position.z / 3.5f), sin(position.z / 3.5f));
  return dot(vec, position.xy) * 0.5f;
}

float Shelves(vec3 position)
{
  return cos(position.z / 12.0f) * 1.5f;
}

// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
//	Simplex 3D Noise
//	by Ian McEwan, Ashima Arts
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}

float snoise(vec3 v){
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

  // First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

  // Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

  // Permutations
  i = mod(i, 289.0 );
  vec4 p = permute( permute( permute(
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

  // Gradients
  // ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

  //Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  // Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

uniform float offset;
uniform float noiseStrength;
uniform vec3 dimensions;

float CalculateDensity()
{
  vec3 position = fs_in.position;
  position.z += offset;

  float f = 0.0f;

  // Add Pillars
  f += Pillars(position);

  // Add Negative Center
  f -= NegativeCenter(position);

  // Add Negative outer edge
  f -= NegativeEdge(position);

  // Add Helix
  f += Helix(position);

  // Add Shelves
  f += Shelves(position);

  // Add Noise
  vec3 noisePos = position;
  noisePos.x = ((noisePos.x + 1.0f) / 2.0f) * dimensions.x;
  noisePos.y = ((noisePos.y + 1.0f) / 2.0f) * dimensions.y;
  f += snoise(noisePos) * noiseStrength;

  return f;
}

void main()
{
  if(fs_in.position.z - offset == 0 || fs_in.position.z - offset == dimensions.z - 1)
  {
    color = vec3(0, 0, 0);
  }
  else
  {
    float f = CalculateDensity();
    color = vec3(f, 0, 0);
  }
}
