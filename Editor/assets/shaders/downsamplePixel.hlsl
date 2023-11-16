#include "Common.hlsli"

cbuffer Params : register(b5) {
	float2 texelSize;
	float delta;
	float intensity;
};

Texture2D tex : register(t0);
SamplerState texSampler : register(s0);

float4 main(VSout input) : SV_TARGET
{
	float2 o = float2(-delta, delta);
	float4 s = tex.Sample(texSampler, input.uv + texelSize * o.xx)
					 + tex.Sample(texSampler, input.uv + texelSize * o.xy)
					 + tex.Sample(texSampler, input.uv + texelSize * o.yy)
					 + tex.Sample(texSampler, input.uv + texelSize * o.yx);
	return s * 0.25 * intensity;
}