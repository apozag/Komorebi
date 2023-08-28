#include "Common.hlsli"

cbuffer Parameters : register(b7){
  float metalness;
  float roughness;
  float reflectivity;
};

struct PS_OUTPUT {
  float4 color: SV_Target0;
  float4 normal: SV_Target1;
  float4 position: SV_Target2;
  float4 pbrParams: SV_Target4;
};

SamplerState texSampler : register(s7);
Texture2D albedoTex : register(t7);
Texture2D normalTex : register(t8);

PS_OUTPUT main(VSout i)
{  
  float3 n = normalize(i.normal);
  float3 t = normalize(i.tangent - dot(i.tangent, n) * n);
  float3 b = cross(i.normal, i.tangent);
  float3x3 tbn = float3x3(t, b, n);

  float3 normal = 2*normalTex.Sample(texSampler, i.uv).xyz - float3(1,1,1);
  normal = normalize(mul(normal, tbn));

  PS_OUTPUT o;
  o.color = albedoTex.Sample(texSampler, i.uv);
  o.normal = float4(normal, 0);
  o.position = i.worldPos;
  o.pbrParams = float4(metalness, roughness, reflectivity, 1);
	return o;
}