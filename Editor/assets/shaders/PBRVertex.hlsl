#include "VertexCommon.hlsli"

VSout main(VSin input) {
	VSout vso;

	float4 worldPos = mul(float4(input.p, 1.0), model);
	vso.pos = mul(worldPos, viewproj);
	vso.worldPos = worldPos;
	vso.normal = normalize(mul(float4(input.n, 0.0), model)).xyz;
	vso.uv = input.uv;
	return vso;
}