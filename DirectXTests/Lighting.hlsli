#ifndef __LIGHTING_HLSL__
#define __LIGHTING_HLSL__

#include "Common.hlsli"

#define MAX_DIRLIGHTS	5
#define MAX_SPOTLIGHTS	5
#define MAX_POINTLIGHTS 5

cbuffer DirLights : register(b0) {
	float4 dirLightColor[MAX_DIRLIGHTS];
	float4 dirLightDir[MAX_DIRLIGHTS];
	uint dirLightCount;
};

cbuffer ShadowInfo : register(b3) {
	float shadowmapSize;
	float shadowmapTexelSize;
	float pcfWindowSize;
	float pcfFilterSize;
	float pcfFilterSizeInv;
}

SamplerState shadowMapSampler : register(s0);
SamplerState pcfSampler : register(s5);

Texture2D shadowMap : register(t0);
Texture3D pcfTex : register(t5);

float3 calcDirLight(float3 normal, int idx) {
	return max(0, dot(normal, -dirLightDir[idx])) * dirLightColor[idx];
}

float3 calcDirLights(float3 normal) {
	float3 c = float3(0, 0, 0);
	for (int i = 0; i < dirLightCount; i++) {
		c += calcDirLight(normal, i);
	} 
	return c;
}

float calcShadow(float4 lightSpacePos) {
	// perform perspective divide and transform to [0,1] range
	float3 projCoords = lightSpacePos.xyz;
	projCoords.x = lightSpacePos.x / lightSpacePos.w * 0.5 + 0.5f;
	projCoords.y = -lightSpacePos.y / lightSpacePos.w * 0.5 + 0.5f;
	projCoords.z = lightSpacePos.z / lightSpacePos.w;
	float wrapped_u = ((projCoords.x * shadowmapSize) % pcfWindowSize) / pcfWindowSize;
	float wrapped_v = ((projCoords.y * shadowmapSize) % pcfWindowSize) / pcfWindowSize;
	float bias = 0.005;
	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;
	float filterSizeInvSqr = pcfFilterSizeInv * pcfFilterSizeInv;
	float sum = 0;
	for (int i = 0; i < pcfFilterSize* pcfFilterSize; i++) {
		float2 offset = pcfTex.Sample(shadowMapSampler, float3(float(i) * filterSizeInvSqr, wrapped_u, wrapped_v)).rg * shadowmapTexelSize;
		float depth = shadowMap.Sample(shadowMapSampler, projCoords.xy + offset).r;
		sum += depth > (currentDepth - bias) ? 1.0 : 0.0;
	}
	return sum * filterSizeInvSqr;
}

#define CALC_DIRLIGHTS(i) calcDirLights(i.normal)
#define CALC_SHADOW(i) calcShadow(i.lightSpacePos)

#endif //__LIGHTING_HLSL__