#define MAX_DIRLIGHTS   5

cbuffer DirLights : register(b0) {
	float4 color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
	uint count;
};

struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 color : COLOR;
};

float4 main(VSout i) : SV_Target
{

	float3 albedo = float3(1,0,0);

	float3 c = i.color;

	c += max(0, dot(i.normal, -dir[0])) * color[0] * albedo;

	c += albedo * 0.3;

	return float4(c, 0.8);
}