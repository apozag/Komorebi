struct GSout {
	float4 pos : SV_Position;
	float3 velocity : VELOCITY;
	float2 uv : TEXCOORD;
	float lifeTime : LIFETIME;
};

cbuffer Params : register(b5) {
	float4 color;
};

Texture2D albedoTex : register(t0);
SamplerState texSampler;

float4 main(GSout input) : SV_Target0
{
	float4 c = albedoTex.Sample(texSampler, input.uv);
	c.rgb *= color;
	c.a *= input.lifeTime;
	return c;
}