#define MAX_DIRLIGHTS   5


cbuffer DirLights : register(b1) {
	uint count;
	float4 m_color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
};

struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD0;
};

Texture2D albedoTex : register(t0);

SamplerState texSampler;

float4 main(VSout i) : SV_Target
{

	float3 albedo = albedoTex.Sample(texSampler, i.uv).xyz;

	float3 c = float3(0,0,0);

	// diffuse
	c += max(0, dot(i.normal, -dir[0])) * m_color[0] * albedo;

	//ambient
	c += albedo * 0.3;

	return float4(c, 0.8);
}