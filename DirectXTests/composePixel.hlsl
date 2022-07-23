struct vs_out {
	float4 pos : SV_Position;
	float2 uv : TEXCOORD;
};

float4 main(vs_out input) : SV_Target0
{
	float4 c = float4(1, 0, 0, 1);// mainTex.Sample(smplr, input.uv);

	return c;
}