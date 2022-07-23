struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 color : COLOR;
};
/*
struct VSin
{
	float3 v : Position;
	float3 n : NORMAL;
	float3 t : TANGENT;
	float2 uv : TexCoord;
	uint4 bones : BONES;
	float4 weights : WEIGHTS;
};
*/

cbuffer CameraTransform : register(b3)
{
	matrix viewproj;
	matrix view;
	matrix proj;
};

cbuffer Modeltransform : register(b5) {
	matrix model;
}

cbuffer BonesTransform : register(b6) {
	matrix bones[60];
}

static matrix Identity =
{
	{ 1, 0, 0, 0 },
	{ 0, 1, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 }
};

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

	vso.pos = mul(mul(float4(v, 1.0), BoneTransform), viewproj);
	//vso.pos = mul(mul(float4(v, 1.0), model), viewproj);
	vso.normal = normalize(mul(float4(n, 0.0), BoneTransform)).xyz;
	vso.color = boneIdx / 50;
	return vso;
}