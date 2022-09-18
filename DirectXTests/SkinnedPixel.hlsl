#define MAX_DIRLIGHTS   5

cbuffer DirLights : register(b0) {
	float4 color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
	uint count;
};

struct VSout
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
	float4 lightSpacePos : LIGHTPOS;
	float3 normal : NORMAL;
};

SamplerState shadowMapSampler : register(s0);
Texture2D shadowMap : register(t0);

//SamplerState albedoSampler : register(s5);
//Texture2D albedoTex : register(t5);

float calcShadow(float4 lightViewPosition) {
	// perform perspective divide
	// transform to [0,1] range
	float3 projCoords = lightViewPosition.xyz;
	projCoords.x = lightViewPosition.x / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.y = -lightViewPosition.y / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.z = lightViewPosition.z / lightViewPosition.w;

	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;

	float closestDepth = shadowMap.Sample(shadowMapSampler, projCoords.xy).r;

	float bias = 0.005;

	return closestDepth > (currentDepth - bias) ? 1.0 : 0.0;
}

float4 main(VSout i) : SV_Target
{

	float3 albedo = float3(1,0,0);//albedoTex.Sample(albedoSampler, i.uv);

	float3 c = float3(0,0,0);

	c += max(0, dot(i.normal, -dir[0])) * color[0] * albedo * calcShadow(i.lightSpacePos);

	c += albedo * 0.3;

	return float4(c, 0.8);
}