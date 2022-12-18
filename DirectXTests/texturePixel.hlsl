#define MAX_DIRLIGHTS   5


cbuffer DirLights : register(b0) {
	float4 m_color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
	uint count;
};

struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

Texture2D albedoTex : register(t3);
Texture2D normalTex : register(t1);

SamplerState texSampler;

float4 main(VSout i) : SV_Target
{
	float3x3 TBN = float3x3(i.tangent, i.bitangent, i.normal);
	float3 surfNormal = normalTex.Sample(texSampler, i.uv).xyz;
	float3 n = mul(surfNormal, TBN);

	float3 albedo = albedoTex.Sample(texSampler, i.uv).xyz;

	float3 c = float3(0,0,0);

	//diffuse
	c += max(0, dot(n, -dir[0])) * m_color[0] * albedo;

	//ambient
	c += albedo * 0.3;

	return float4(c, 0.8);
}