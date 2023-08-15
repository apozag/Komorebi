#define _PI  3.1416f
#define _2PI 6.2832f
#define _PI2 1.5708f

TextureCube	EnvMap : register(t13);

SamplerState LinearSampler : register(s0);

struct g2f {
  float4 pos : SV_POSITION;
  float4 worldPos : WORLD_POS;
  uint targetIdx : SV_RenderTargetArrayIndex;
};

float4 main(g2f input) : SV_Target0
{
  static const float angleDelta = 0.01f;

  float3 normal = normalize(input.worldPos.xyz);
  float3 up = float3(0.0, 1.0, 0.0);
  float3 right = normalize(cross(up, normal));
  up = normalize(cross(normal, right));

  float3 irradiance;
  int sampleCount = 0;
  for (float phi = 0.f; phi < _2PI; phi += angleDelta) {
    for (float theta = 0.f; theta < _PI2; theta += angleDelta) {

      float sinTheta = sin(theta);
      float cosTheta = cos(theta);

      // spherical to cartesian(in tangent space)
      float3 tangentSample = float3(sinTheta * cos(phi), sinTheta * sin(phi), cosTheta);
      // tangent space to world
      float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

      irradiance += EnvMap.Sample(LinearSampler, sampleVec).rgb * cosTheta * sinTheta;

      ++sampleCount;
    }
  }  
  irradiance *= _PI / (float)sampleCount;
  return float4(irradiance, 1.0f);
}
