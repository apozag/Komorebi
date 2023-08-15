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
	float3 m_color : COLOR;
};

float4 main(VSout i) : SV_Target
{

	float3 albedo = float3(1,0,0);

	//float3 c = i.m_color;
	float3 c = float3(1,1,1);

	c += max(0, dot(i.normal, dir[0])) * m_color[0] * albedo;

	c += albedo * 0.3;

	return float4(c, 0.8);
}