#define MAX_DIRLIGHTS   5

cbuffer DirLights : register(b0) {
	float4 m_color[MAX_DIRLIGHTS];
	float4 dir[MAX_DIRLIGHTS];
	uint count;
};

struct VSout
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
	float4 lightSpacePos : LIGHTPOS;
	float3 normal : NORMAL;
	float3 m_color : COLOR;
};

SamplerState shadowMapSampler : register(s0);
Texture2D shadowMap : register(t0);

SamplerState albedoSampler : register(s7);
Texture2D albedoTex : register(t7);
Texture3D pcfTex : register(t6);

float3 calcShadow(float4 lightViewPosition) {
	// perform perspective divide
	// transform to [0,1] range

	float3 projCoords = lightViewPosition.xyz;
	projCoords.x = lightViewPosition.x / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.y = -lightViewPosition.y / lightViewPosition.w * 0.5 + 0.5f;
	projCoords.z = lightViewPosition.z / lightViewPosition.w;
	float wrapped_u = ((projCoords.x * 1024) % 256) / 256;
	float wrapped_v = ((projCoords.y * 1024) % 256) / 256;
	float sum = 0;
	float bias = 0.005;
	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;
	for (int i = 0; i < 25; i++) {
		float2 offset = pcfTex.Sample(shadowMapSampler, float3(float(i + 1) / 25, wrapped_u, wrapped_v)) / 1024;
		sum += shadowMap.Sample(shadowMapSampler, projCoords.xy + offset).r > (currentDepth - bias) ? 1.0 : 0.0;
	}

	return sum / 25;
}

float4 main(VSout i) : SV_Target
{

	float3 albedo = albedoTex.Sample(albedoSampler, i.uv);

	float3 c = float3(0,0,0);

	c += max(0, dot(i.normal, -dir[0])) * m_color[0] * albedo;// *calcShadow(i.lightSpacePos);

	c += albedo * 0.3;

	return float4(c, 0.8);
}