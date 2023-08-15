#include "Lighting.hlsli"

Texture2D colorTex : register(t7);
Texture2D normalTex : register(t8);
Texture2D posTex : register(t9);

SamplerState texSampler;

float4 main(VSout input) : SV_Target0
{
	float3 albedo = colorTex.Sample(texSampler, input.uv).rgb;
	input.normal = normalTex.Sample(texSampler, input.uv).rgb;
	float4 worldPos = posTex.Sample(texSampler, input.uv);	
	float3 c = CALC_SHADOW(worldPos) * CALC_DIRLIGHT(input) * albedo;
	return float4(c, 1);
}