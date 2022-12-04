struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
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

VSout main(float3 v : Position, float3 n : NORMAL, float3 t : TANGENT, float2 uv : TexCoord)
{
	VSout vso;
	vso.pos = mul(mul(float4(v, 1.0), model), viewproj);
	vso.normal = normalize(mul(float4(n, 0.0), model)).xyz;
	vso.tangent = normalize(mul(float4(t, 0.0), model)).xyz;
	vso.bitangent = dot(vso.normal, vso.tangent);
	vso.uv = uv;
	return vso;
}