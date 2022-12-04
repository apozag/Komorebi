#ifndef __COMMON_HLSL__
#define __COMMON_HLSL__

struct VSout
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
	float4 lightSpacePos : LIGHTPOS;
	float4 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

#endif // __COMMON_HLSL__