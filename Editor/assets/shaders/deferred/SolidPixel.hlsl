#include "Lighting.hlsli"

struct PS_OUTPUT {
  float4 color: SV_Target0;
  float4 normal: SV_Target1;
  float4 position: SV_Target2;
};

PS_OUTPUT main(VSout i) : SV_Target
{
  PS_OUTPUT o;
  o.color = float4(0,0,0,1);
  o.normal = float4(i.normal.x, i.normal.y, i.normal.z, 0);
  o.position = i.worldPos;
  return o;
}