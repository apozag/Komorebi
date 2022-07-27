#define MAX_DIRLIGHTS   5


cbuffer DirLights : register(b0) {
	float4 color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
	uint count;
};

struct VSout
{
	float4 pos : SV_Position;
	float4 lightSpacePos : LIGHTPOS;
	float4 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

SamplerState shadowMapSampler : register(s5);
Texture2D shadowMap : register(t5);

float3 calcShadow(float4 lightViewPosition) {
	// perform perspective divide
	// transform to [0,1] range
	float3 projCoords = lightViewPosition.xyz;
	projCoords.x =  lightViewPosition.x / lightViewPosition.w *0.5 + 0.5f;
	projCoords.y = -lightViewPosition.y / lightViewPosition.w *0.5 + 0.5f;
	projCoords.z =  lightViewPosition.z / lightViewPosition.w;

	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;

	float closestDepth = shadowMap.Sample(shadowMapSampler, projCoords.xy).r;

	float bias = 0.005;

	return closestDepth > (currentDepth-bias) ? 1.0 : 0.0;
}

float4 main(VSout i) : SV_Target
{
	
	float3 albedo = float3(1,1,1);

	float3 c = float3(0,0,0);

	//diffuse
	c += max(0, dot(i.normal, -dir[0])) * color[0] * albedo * calcShadow(i.lightSpacePos);

	//ambient
	c += albedo * 0.3;
	
	return float4(c, 1);
};