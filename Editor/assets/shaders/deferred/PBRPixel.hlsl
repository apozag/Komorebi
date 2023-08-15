#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float metalness;
  float roughness;
  //float4 albedoAdditive = float4(0, 0, 0, 0);
};

struct PS_OUTPUT {
  float4 color: SV_Target0;
  float4 normal: SV_Target1;
  float4 position: SV_Target2;
  float4 pbrParams: SV_Target4;
};

SamplerState texSampler : register(s7);
Texture2D albedoTex : register(t7);

PS_OUTPUT main(VSout i)
{
  PS_OUTPUT o;
  o.color = albedoTex.Sample(texSampler, i.uv);// +albedoAdditive;
  o.normal = normalize(float4(i.normal.xyz, 0));
  o.position = i.worldPos;
  o.pbrParams = float4(metalness, roughness, 0, 1);
  return o;
}