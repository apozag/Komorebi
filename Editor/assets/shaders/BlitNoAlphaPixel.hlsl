#include "Common.hlsli"

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

float4 main(VSout input) : SV_TARGET
{
	return float4(tex.Sample(texSampler, input.uv).xyz, 1);
	//return float4(1,1,1,1);
}