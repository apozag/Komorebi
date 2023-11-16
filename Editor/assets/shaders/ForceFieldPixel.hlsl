#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float4 color;
  float2 uvMult;
  float fresnellExp1;
  float fresnellExp2;
  float attenuation;
  float animSpeed;
  float contactDist;
};

SamplerState texSampler : register(s7);
Texture2D noiseTex : register(t7);

Texture2D depthTex : register(t9);

float4 main(VSout i) : SV_Target0{
  float backFace = (float) i.normal.z < 0.f;
  //float3 normal = i.normal * (1-backFace) - i.normal * backFace;  
  //float3 viewDir = normalize(float4(viewInv._14, viewInv._24, viewInv._34, 1) - i.worldPos).xyz;
  float VdotN = abs(dot(float3(0, 0, 1), i.normal));
  float fresnell1 = pow(1 - VdotN, fresnellExp1);
  float fresnell2 = pow(1 - VdotN, fresnellExp2);

  float2 uv1 = i.uv * uvMult + float2(time, time) * animSpeed;
  float2 uv2 = i.uv * uvMult + float2(-time + 0.5, time) * animSpeed;
  float noise1 = noiseTex.Sample(texSampler, uv1).r;
  float noise2 = noiseTex.Sample(texSampler, uv2).r;

  float4 clipCoords = mul(i.worldPos, viewproj);
  float3 projCoords;
  projCoords.x = (clipCoords.x / clipCoords.w) * 0.5 + 0.5f;
  projCoords.y = 1 - (clipCoords.y / clipCoords.w) * 0.5 + 0.5f;
  projCoords.z = (clipCoords.z / nfwh.y);
  float currDepth = projCoords.z;

  float4 bgWorldPos = depthTex.Sample(texSampler, projCoords.xy);
  float4 bgProjPos = mul(bgWorldPos, viewproj);
  float bgDepth = bgProjPos.z / nfwh.y;
  
  float notSkybox = 1-(float)(bgDepth == 0);
  float dist = bgDepth - currDepth;
  float contact = notSkybox * (1 - smoothstep(0, contactDist / nfwh.y, dist));

  float4 c = float4(color.rgb * 0.5f, fresnell1 * noise1/* + fresnell1 * noise2*/ + (fresnell2 + contact) * attenuation);
  
  return c;
  /*float a = (float)(projCoords.x > 0.5);
  float d = a * bgDepth + (1 - a) * currDepth;
  return float4(d,d,d, 1.0);*/

  //return float4(contact, contact, contact, 1.0);
}