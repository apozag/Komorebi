#ifndef __COMMON_HLSL__
#define __COMMON_HLSL__

#define _PI 3.14159

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

cbuffer global : register(b0) {
	float time;
};

cbuffer CameraTransform : register(b1) {
	matrix viewproj;
	matrix view;
	matrix proj;
	matrix viewInv;
	float4 nfwh;
};

#endif // __COMMON_HLSL__