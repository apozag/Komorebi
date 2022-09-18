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

cbuffer CameraTransform : register(b0)
{
	matrix viewproj;
	matrix view;
	matrix proj;
};

cbuffer Modeltransform : register(b1) {
	matrix model;
}

cbuffer LightTransform : register(b3) {
	matrix lightViewProj[5];
}

VSout main(float3 v : Position, float3 n : NORMAL, float3 t : TANGENT, float2 uv : TexCoord)
{
	VSout vso;
	vso.worldPos = mul(float4(v, 1.0), model);
	vso.pos = mul(vso.worldPos, viewproj);
	vso.lightSpacePos = mul(vso.worldPos, lightViewProj[0]);
	vso.normal = normalize(mul(float4(n, 0.0), model)).xyz;
	vso.tangent = normalize(mul(float4(t, 0.0), model)).xyz;
	vso.bitangent = dot(vso.normal, vso.tangent);
	vso.uv = uv;
	return vso;
}