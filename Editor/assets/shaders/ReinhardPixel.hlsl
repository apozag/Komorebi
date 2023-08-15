#include "Common.hlsli"

cbuffer params : register(b7) {
	float whitePoint = 1;
};

Texture2D hdrTex : register(t0);
SamplerState texSampler : register(s0);

float calcLuminance(float3 color) {
	return 0.2126*color.r + 0.7152*color.g + 0.0722 * color.b;
}

float4 main(VSout input) : SV_TARGET
{
	float3 color = hdrTex.Sample(texSampler, input.uv).rgb;

	float lIn = calcLuminance(color);

	float lOut = lIn * (1 + (lIn / (whitePoint * whitePoint))) / (1 + lIn);

	return float4(color * (lOut / lIn), 1);

}