struct VSout
{
	float4 pos : SV_Position;
	float4 lightSpacePos : LIGHTPOS;
	float3 normal : NORMAL;
	float3 color : COLOR;
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

cbuffer BonesTransform : register(b2) {
	matrix bones[60];
}

cbuffer LightTransform : register(b3) {
	matrix lightViewProj[5];
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

	float4 worldPos = mul(float4(v, 1.0), BoneTransform);
	vso.pos = mul(worldPos, viewproj);
	vso.lightSpacePos = mul(worldPos, lightViewProj[0]);
	vso.normal = normalize(mul(float4(n, 0.0), BoneTransform)).xyz;
	vso.color = boneIdx / 50;
	return vso;
}