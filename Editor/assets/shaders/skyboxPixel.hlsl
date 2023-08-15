TextureCube	EnvMap : register(t12);

SamplerState LinearSampler : register(s0);

struct VSOutput
{
    float3 TexCoord : TEXCOORD;
};


float4 main(VSOutput input) : SV_Target3
{	
	return EnvMap.Sample(LinearSampler, normalize(input.TexCoord));
}