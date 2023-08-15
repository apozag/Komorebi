/*
#define MAX_DIRLIGHTS   5

cbuffer DirLights : register(b0) {
	uint count;
	struct DLData {
		float3 dir;
		float3 color;
	} buffer[MAX_DIRLIGHTS];
	int padding;
};
*/

struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD0;
};

float4 main(VSout i) : SV_Target
{
	//float3 albedo = float3(1, 0, 0);

	//float3 c = albedo;

	//diffuse
	//c *= max(0, dot(i.normal, -buffer[0].dir)) * buffer[0].color;

	//ambient
	//c += albedo*0.5;

	return float4(1.0, 0.0, 0.0, 1.0);
}