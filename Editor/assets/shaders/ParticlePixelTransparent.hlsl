struct GSout {
	float4 pos : SV_Position;
	float3 velocity : VELOCITY;
	float2 uv : TEXCOORD;
	float lifeTime : LIFETIME;
};

Texture2D albedoTex : register(t0);
SamplerState texSampler;

float4 main(GSout input) : SV_Target0
{
	float4 color = albedoTex.Sample(texSampler, input.uv);
	color.rgb = color.rgb * float3(1, 1, 0);
	color.a *= input.lifeTime;
	return color;
}