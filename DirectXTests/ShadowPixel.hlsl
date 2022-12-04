#define MAX_DIRLIGHTS   5


cbuffer DirLights : register(b0) {
	float4 lightColor[MAX_DIRLIGHTS];
	float4 lightDir[MAX_DIRLIGHTS];
	uint count;
};

/*cbuffer Color : register(b6) {
	float4 color;
}*/

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

SamplerState shadowMapSampler : register(s0);
SamplerState pcfSampler : register(s5);
Texture2D shadowMap : register(t0);
Texture3D pcfTex : register(t5);

float2 calcShadow(float4 lightViewPosition) {
	// perform perspective divide
	// transform to [0,1] range
	float3 projCoords = lightViewPosition.xyz;
	projCoords.x = lightViewPosition.x / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.y = -lightViewPosition.y / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.z = lightViewPosition.z / lightViewPosition.w;
	float wrapped_u = ((projCoords.x * 1024) % 32) / 32;
	float wrapped_v = ((projCoords.y * 1024) % 32) / 32;
	float sum = 0;
	float bias = 0.005;
	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;
	//return pcfTex.Sample(pcfSampler, float3(wrapped_u, wrapped_u, wrapped_v)).rg;
	
	for (int i = 0; i < 25; i++) {
		float2 offset = pcfTex.Sample(shadowMapSampler, float3(float(i) / 25, wrapped_u/500, wrapped_v/500)).rg / 1024;
		sum += shadowMap.Sample(shadowMapSampler, projCoords.xy + offset).r > (currentDepth - bias) ? 1.0 : 0.0;
	}
	return sum / 25;	
}

float4 main(VSout i) : SV_Target
{

	return float4(calcShadow(i.lightSpacePos), 0, 1);
/*
	float3 albedo = float3(1,1,1);

	float3 c = float3(0,0,0);

	//diffuse
	c += max(0, dot(i.normal, -lightDir[0])) * lightColor[0] * albedo * calcShadow(i.lightSpacePos);

	//ambient
	c += albedo * 0.3;
	
	return float4(c, 1);
	*/
};