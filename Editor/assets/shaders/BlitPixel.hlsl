#include "Common.hlsli"

Texture2D tex : register(t10);
SamplerState texSampler : register(s10);

float4 main(VSout input) : SV_TARGET
{
	return tex.Sample(texSampler, input.uv);
}