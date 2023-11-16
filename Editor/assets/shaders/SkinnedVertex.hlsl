#include "VertexCommon.hlsli"

cbuffer BonesTransform : register(b3) {
	matrix bones[60];
}

VSout main(float3 v		: Position,
		   float3 n		: NORMAL,
		   float3 t		: TANGENT,
		   float2 uv	: TexCoord,
		   uint4  boneIdx : BONES,
		   float4 weights : WEIGHTS)
{
	VSout vso;

	matrix BoneTransform = bones[boneIdx.x] * weights.x;
	BoneTransform		+= bones[boneIdx.y] * weights.y;
	BoneTransform		+= bones[boneIdx.z] * weights.z;
	BoneTransform		+= bones[boneIdx.w] * weights.w;

	float4 worldPos = mul(float4(v, 1.0), BoneTransform);
	vso.pos = mul(worldPos, viewproj);
	vso.worldPos = worldPos;
	vso.normal = normalize(mul(float4(n, 0.0), BoneTransform)).xyz;
	vso.tangent = normalize(mul(float4(t, 0.0), BoneTransform)).xyz;
	vso.uv = uv;
	return vso;
}