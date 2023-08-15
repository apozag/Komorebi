//#include "Commmon.hlsli"

struct g2f {
  float4 pos : SV_POSITION;
  float4 worldPos : WORLD_POS;
  uint targetIdx : SV_RenderTargetArrayIndex;
};

Texture2D equirectangularMap : register(t0);
SamplerState texSampler : register(s0);

const float2 invAtan = float2(0.3183, 0.6366);
float2 SampleSphericalMap(float3 v) {
  float2 uv = float2(atan2(v.z, v.x) * 0.3183, v.y);
  uv += 1;
  uv *= 0.5;
  return uv;
}

float4 main(g2f input) : SV_TARGET{
  input.worldPos.y *= -1;
  float2 uv = SampleSphericalMap(normalize(input.worldPos)); 
  float3 color = equirectangularMap.Sample(texSampler, uv).rgb *50;

  return float4(color, 1);
}
