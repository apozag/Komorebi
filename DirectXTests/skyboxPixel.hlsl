TextureCube	EnvMap : register(t0);

SamplerState LinearSampler : register(s0);

struct VSOutput
{
    float3 TexCoord : TEXCOORD;
};


float4 main(VSOutput input) : SV_Target0
{	
	return EnvMap.Sample(LinearSampler, normalize(input.TexCoord));
}