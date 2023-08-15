
struct VSout
{
	float4 pos : SV_Position;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float2 uv : TEXCOORD0;
};

VSout main(float3 pos : Position, float3 n : NORMAL, float2 uv : TexCoord) {
	VSout o;
	o.pos = float4(pos, 1);
	o.normal = 0;
	o.tangent = 0;
	o.bitangent = 0;
	o.uv = o.pos * float2(0.5,0.5) + float2(0.5,0.5);
	return o;
}