
struct vs_out {
  float4 pos: SV_Position;
  float2 uv: TexCoord;
};
vs_out main(float3 pos : Position, float3 n : NORMAL, float2 uv : TexCoord) {
  vs_out outp;
  outp.pos = float4(pos, 1);
  outp.uv = uv;
  return outp;
}