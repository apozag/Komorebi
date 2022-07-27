struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

Texture2D mainTex : register(t0);

SamplerState texSampler;

float4 main(VSout input) : SV_Target0
{
	float4 c =  mainTex.Sample(texSampler, input.uv);

	return c;
}