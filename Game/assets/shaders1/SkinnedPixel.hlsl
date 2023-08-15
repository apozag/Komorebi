#include "Lighting.hlsli"

SamplerState albedoSampler : register(s6);
Texture2D albedoTex : register(t6);

float4 main(VSout i) : SV_Target
{

	float3 albedo = albedoTex.Sample(albedoSampler, i.uv);

	float3 c = float3(0, 0, 0);
	c += CALC_DIRLIGHTS(i) * albedo * CALC_SHADOW(i);

	c += albedo * 0.3;

	return float4(c, 0.8);
}