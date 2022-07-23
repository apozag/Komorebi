cbuffer Light {
	float4 dir;
	float4 color;
};

struct vs_out {
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D emissiveTex : register(t2);

SamplerState smplr;

float4 main(vs_out input) : SV_Target0
{
    float4 albedo = albedoTex.Sample(smplr, input.uv);
    float3 normal = normalTex.Sample(smplr, input.uv);
	float4 emissive = emissiveTex.Sample(smplr, input.uv);

	float4 c = albedo;

	//diffuse
	c *= max(0, dot(normal, -dir)) * color;

	//ambient
	c += albedo * 0.3;

	//emissive
	c += emissive;

	return c;
}