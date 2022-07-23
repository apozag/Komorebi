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
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

Texture2D shadowMap : register(t0);

float calcShadow() {
	// perform perspective divide
	vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light’s perspective
	float currentDepth = projCoords.z > 1.0 ? 0.0 : projCoords.z;

	float closestDepth = texture(shadowMap, projCoords.xy).r;

	float bias = 0.005;

	return currentDepth - bias > closestDepth ? 0.0 : 1.0;
}

float4 main(VSout i) : SV_Target
{
	float3x3 TBN = float3x3(i.tangent, i.bitangent, i.normal);
	float3 surfNormal = normalTex.Sample(texSampler, i.uv).xyz;
	float3 n = mul(surfNormal, TBN);

	float3 albedo = float3(1,1,1,1)

	float3 c = float3(0,0,0);

	//diffuse
	c += max(0, dot(n, -dir[0])) * color[0] * albedo * clacShadow();

	//ambient
	c += albedo * 0.3;

	return float4(c, 1);
};