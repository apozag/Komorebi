#include "Common.hlsli"

cbuffer Parameters : register(b7) {
  float4 color0;
  float4 color1;
  float2 ellipse;
  float2 uvMult;
  float threshold;
  float smooth;
  float animSpeed;  
};

SamplerState texSampler : register(s7);
Texture2D noiseTex : register(t7);

float4 main(VSout i) : SV_Target0
{  

float2 uv1 = i.uv * uvMult + float2(0, time) * animSpeed;
float2 uv2 = i.uv * uvMult + float2(0, time) * animSpeed * 0.2;
float noise1 = noiseTex.Sample(texSampler, uv1).r;
float noise2 = noiseTex.Sample(texSampler, uv2).r;

noise1 = smoothstep(0, smooth, saturate(noise1 - threshold));
noise2 = smoothstep(0, smooth, saturate(noise2 - threshold));

float u = (i.uv.x - 0.5) * 2;
float v = (i.uv.y * i.uv.y - 0.5) * 2;

float sdf = 1 - saturate((pow(u, 2) / pow(ellipse.x, 2)) + (pow(v, 2) / pow(ellipse.y, 2)));

float3 c = sdf * color0.rgb + (1 - sdf) * color1.rgb;

float a = sdf * noise1 * noise2 * i.uv.y;

return float4(c, a);

}