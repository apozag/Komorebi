#ifndef __VERTEXCOMMON_HLSL__
#define __VERTEXCOMMON_HLSL__

#include "Common.hlsli"


struct VSin {
	float3 p : Position;
	float3 n : NORMAL;
	float3 t : TANGENT;
	float2 uv : TexCoord;
};

cbuffer CameraTransform : register(b0)
{
	matrix viewproj;
	matrix view;
	matrix proj;
};

cbuffer Modeltransform : register(b1) {
	matrix model;
}

#define WORLD_POS(i) mul(float4(i.p, 1.0), model)
#define WORLD_TO_PROJ(p) mul(vso.worldPos, viewproj)
#define WORLD_TO_LIGHT(p, idx) mul(p, lightViewProj[idx])

#endif // __VERTEXCOMMON_HLSL