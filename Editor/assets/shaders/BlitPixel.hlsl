#include "Common.hlsli"

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

float4 main(VSout input) : SV_TARGET
{
	return tex.Sample(texSampler, input.uv);
}