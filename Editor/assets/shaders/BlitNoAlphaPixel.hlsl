#include "Common.hlsli"

Texture2D tex : register(t10);
SamplerState texSampler : register(s10);

float4 main(VSout input) : SV_TARGET
{
	return float4(tex.Sample(texSampler, input.uv).xyz, 1);
}