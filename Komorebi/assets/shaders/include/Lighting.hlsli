#ifndef __LIGHTING_HLSL__
#define __LIGHTING_HLSL__

#include "Common.hlsli"

cbuffer DirLights : register(b1) {
	float4 dirLightColor;
	float4 dirLightDir;
};

cbuffer LightTransform : register(b2) {
	matrix lightViewProj;
}

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
Texture3D pcfTex : register(t6);

float3 calcDirLight(float3 normal) {
	return max(0, dot(normal, dirLightDir.xyz)) * dirLightColor.rgb;
}
//
//float3 calcSpotLight(float4 worldPos, float3 normal, int idx) {
//	float4 d = spotLightDir[idx];
//	float4 c = spotLightColor[idx];
//	float4 p = spotLightPos[idx];
//	float4 wpp = normalize(p - worldPos);
//	float cosTheta = abs(dot(wpp, d));
//	bool cutout = cosTheta < 0.7;
//	return max(0, dot(normal, -d.xyz)) * c.rgb * cutout;
//}
//
//float3 calcDirLights(float3 normal) {
//	float3 c = float3(0, 0, 0);
//	for (unsigned int i = 0; i < dirLightCount; i++) {
//		c += calcDirLight(normal, i);
//	} 
//	return c;
//}
//
//float3 calcSpotLights(float4 worldPos, float3 normal) {
//	float3 c = float3(0, 0, 0);
//	for (unsigned int i = 0; i < dirLightCount; i++) {
//		c += calcSpotLight(worldPos, normal, i);
//	}
//	return c;
//}

float calcShadow(float4 worldPos) {
	float4 lightSpacePos = mul(worldPos, lightViewProj);
	// perform perspective divide and transform to [0,1] range
	float3 projCoords = lightSpacePos.xyz;
	projCoords.x = lightSpacePos.x / lightSpacePos.w * 0.5 + 0.5f;
	projCoords.y = 1-(lightSpacePos.y / lightSpacePos.w * 0.5 + 0.5f);
	projCoords.z = lightSpacePos.z / lightSpacePos.w;

	if (projCoords.x > 1 || projCoords.x < 0 || projCoords.y > 1 || projCoords.y < 0) {
		return 1;
	}

	float wrapped_u = ((projCoords.x * shadowmapSize) % pcfWindowSize) / pcfWindowSize;
	float wrapped_v = ((projCoords.y * shadowmapSize) % pcfWindowSize) / pcfWindowSize;
	float bias = 0.005;
	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;
	float filterSizeInvSqr = pcfFilterSizeInv * pcfFilterSizeInv;
	float sum = 0;
	for (int i = 0; i < 25; i++) {
		float2 offset = pcfTex.Sample(shadowMapSampler, float3(float(i) * filterSizeInvSqr, wrapped_u, wrapped_v)).rg * shadowmapTexelSize;
		float depth = shadowMap.Sample(shadowMapSampler, projCoords.xy + offset).r;
		sum += depth > (currentDepth - bias) ? 1.0 : 0.0;
	}
	return sum * filterSizeInvSqr;
}

#define CALC_SPOTLIGHTS(i) float3(0,0,0)//calcSpotLights(i.worldPos, i.normal)
#define CALC_DIRLIGHTS(i) float3(0,0,0)//calcDirLights(i.normal)
#define CALC_SHADOW(pos) calcShadow(pos)

#define CALC_DIRLIGHT(i) calcDirLight(i.normal)

#endif //__LIGHTING_HLSL__