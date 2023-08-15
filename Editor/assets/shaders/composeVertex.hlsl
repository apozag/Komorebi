#include "VertexCommon.hlsli"

VSout main(VSin i) {
	VSout o;
	o.pos = float4(i.p, 1);
	o.normal = 0;
	o.tangent = 0;
	o.bitangent = 0;
	o.uv = float2(i.p.x, - i.p.y) * float2(0.5,0.5) + float2(0.5,0.5);
	return o;
}