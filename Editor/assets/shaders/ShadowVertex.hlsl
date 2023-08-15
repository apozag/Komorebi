#include "VertexCommon.hlsli"

VSout main(VSin i)
{
	VSout vso;
	vso.worldPos = WORLD_POS(i);
	vso.pos = WORLD_TO_PROJ(vso.worldPos);
	//vso.lightSpacePos = WORLD_TO_LIGHT(vso.worldPos, 0);
	vso.normal = normalize(mul(float4(i.n, 0.0), model)).xyz;
	vso.tangent = normalize(mul(float4(i.t, 0.0), model)).xyz;
	vso.bitangent = dot(vso.normal, vso.tangent);
	vso.uv = i.uv;
	return vso;
}