#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float metalness;
  float roughness;
  float reflectivity;
  float4 color;
};

struct PS_OUTPUT {
  float4 color: SV_Target0;
  float4 normal: SV_Target1;
  float4 position: SV_Target2;
  float4 pbrParams: SV_Target4;
};

PS_OUTPUT main(VSout i) {
  PS_OUTPUT o;
  o.color = color;
  o.normal = normalize(float4(i.normal.xyz, 0));
  o.position = i.worldPos;
  o.pbrParams = float4(metalness, roughness, reflectivity, 1);
  return o;
}