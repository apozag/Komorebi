#include "Lighting.hlsli"

float4 main(VSout i) : SV_Target
{

	float3 albedo = float3(1,1,1);

	float3 c = float3(0,0,0);

	//diffuse
	c += CALC_DIRLIGHTS(i) * albedo * CALC_SHADOW(i.pos);

	//ambient
	c += albedo * 0.3;

	return float4(c, 1);
};