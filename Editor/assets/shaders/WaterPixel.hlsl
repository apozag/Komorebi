#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float4 color;
  float4 sparkColor;  
  float4 foamColor;  
  float2 animSpeed;
  float fade;
  float threshold;
  float2 uvMult;
  float2 foamUVMult;
  float contactDist;
  float contactThreshold;  
  float smooth;
};

SamplerState texSampler : register(s7);
Texture2D noiseTex1 : register(t7);
Texture2D noiseTex2 : register(t8);

Texture2D depthTex : register(t9);

float4 main(VSout i) : SV_Target0{

float4 clipCoords = mul(i.worldPos, viewproj);
float3 projCoords;
projCoords.x = (clipCoords.x / clipCoords.w) * 0.5 + 0.5f;
projCoords.y = 1 - (clipCoords.y / clipCoords.w) * 0.5 + 0.5f;
projCoords.z = (clipCoords.z / nfwh.y);
float currDepth = projCoords.z;

float4 bgWorldPos = depthTex.Sample(texSampler, projCoords.xy);
float4 bgProjPos = mul(bgWorldPos, viewproj);
float bgDepth = bgProjPos.z / nfwh.y;

float dist = bgDepth - currDepth;
float contact = smoothstep(0, contactDist / nfwh.y, dist);

float2 uv = i.uv;
uv.y = contact;

float2 uv1 = uv * foamUVMult + float2(time, time) * animSpeed;
float2 uv2 = uv * foamUVMult  + float2(0.3, 0) + float2(-time, time) * animSpeed * 0.5;
float2 uv3 = i.uv * uvMult + float2(time, time) * animSpeed;
float2 uv4 = i.uv * uvMult + float2(0.3, 0) + float2(-time, time) * animSpeed * 0.5;

float noise1 = noiseTex2.Sample(texSampler, uv1).r;
float noise2 = noiseTex2.Sample(texSampler, uv2).g;
float noise3 = noiseTex1.Sample(texSampler, uv3).r;
float noise4 = noiseTex1.Sample(texSampler, uv4).r;

noise1 = smoothstep(0, smooth, saturate(noise1 - contactThreshold - contact));
noise2 = smoothstep(0, smooth, saturate(noise2 - contactThreshold - contact));
noise3 = smoothstep(0, smooth, saturate(noise3 - threshold));
noise4 = smoothstep(0, smooth, saturate(noise4 - threshold));

float foamFactor = saturate(noise1 + noise2);
float sparkFactor = noise3 * noise4;

float4 waterColor = float4(color.rgb, saturate(contact / fade + 0.3));
float4 waterSparkColor = waterColor * max(0, (1 - sparkFactor)) + sparkColor * sparkFactor;
float4 finalColor = waterSparkColor * max(0, (1 - foamFactor)) + foamColor * foamFactor;

return finalColor;
}