#include "Common.hlsli"

cbuffer Params : register(b5) {
	float threshold;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

float4 main(VSout input) : SV_TARGET
{
	float4 color = tex.Sample(texSampler, input.uv);	
	float luminance = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
	return color * max(0.f, (luminance - threshold) / max(luminance, 0.00001));
}