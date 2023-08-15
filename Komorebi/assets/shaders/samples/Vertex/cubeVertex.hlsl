struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

cbuffer Constant : register(b0)
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
	vso.normal = normalize(mul(float4(n, 1.0), view)).xyz;
	vso.uv = uv;
	return vso;
}