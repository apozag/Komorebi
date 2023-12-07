#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float4 color;
  float animSpeed;
  float threshold;
  float smooth;
};

SamplerState texSampler : register(s7);
Texture2D noiseTex : register(t7);

float4 main(VSout i) : SV_Target0
{

float2 uv1 = i.uv + float2(0, time) * animSpeed;
float2 uv2 = i.uv + float2(0, time) * animSpeed * 0.5;

float fade = 1 - (i.uv.y * i.uv.y);

float noise1 = noiseTex.Sample(texSampler, uv1).r * fade;
float noise2 = noiseTex.Sample(texSampler, uv2).g * fade;

noise1 = smoothstep(0, smooth, saturate(noise1 - threshold));
noise2 = smoothstep(0, smooth, saturate(noise2 - threshold));

return float4(color.rgb, saturate(noise1 + noise2));

}