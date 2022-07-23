struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

struct MRT_OUT {
	float4 albedo : COLOR0;
	float4 normal : COLOR1;
};

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);

SamplerState texSampler;

MRT_OUT main(VSout i) : SV_TARGET
{
	MRT_OUT mrtOut;

	mrtOut.albedo = albedoTex.Sample(texSampler, i.uv);

	float3x3 TBN = float3x3(i.tangent, i.bitangent, i.normal);
	float3 surfNormal = normalTex.Sample(texSampler, i.uv).xyz;
	mrtOut.normal = float4(mul(surfNormal, TBN), 1.0);

	return mrtOut;
}