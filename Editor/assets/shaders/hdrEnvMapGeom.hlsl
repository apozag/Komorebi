struct g2f {
	float4 pos : SV_POSITION;
	float4 worldPos : WORLD_POS;
	uint targetIdx : SV_RenderTargetArrayIndex;
};

cbuffer ViewProjArr : register(b0) {
	matrix viewProj[6];
}

[maxvertexcount(18)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< g2f > output
)
{
	for (uint i = 0; i < 6; i++) {
		g2f element;
		element.targetIdx = i;
		for (uint j = 0; j < 3; j++) {			
			element.worldPos = input[j];
			element.pos = mul(input[j], viewProj[i]);						
			output.Append(element);
		}
		output.RestartStrip();
	}
}