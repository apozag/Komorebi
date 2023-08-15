#include "VertexCommon.hlsli"

VSout main(float3 v : Position, float3 n : NORMAL, float3 t : TANGENT, float2 uv : TexCoord)
{
	/*VSout vso;
	vso.pos = mul(mul(float4(v, 1.0), model), viewproj);
	vso.normal = normalize(mul(float4(n, 1.0), view)).xyz;
	vso.uv = uv;*/

	VSout vso;
	float4 worldPos = mul(float4(v, 1.0), model);
	vso.pos = mul(worldPos, viewproj);
	vso.worldPos = worldPos;
	vso.normal = normalize(mul(float4(n, 0.0), model)).xyz;
	vso.uv = uv;
	return vso;
}