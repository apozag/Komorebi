#define dot(A, B) max(dot(A, B), 0)

#include "PBR.hlsli"

Texture2D colorTex : register(t7);
Texture2D normalTex : register(t8);
Texture2D posTex : register(t9);
//Texture2D emissiveTex : register(t10);
Texture2D pbrParamsTex : register(t11);

SamplerState texSampler;

float4 main(VSout input) : SV_Target0
{
  PBRinput pbrIn;
  pbrIn.worldPos = posTex.Sample(texSampler, input.uv);
  pbrIn.normal = normalTex.Sample(texSampler, input.uv);
  pbrIn.viewDir = normalize(float4(viewInv._14, viewInv._24, viewInv._34, 1) - pbrIn.worldPos);
  pbrIn.halfVector = normalize((pbrIn.viewDir + dirLightDir) * 0.5);
  pbrIn.lightDir = dirLightDir;
  pbrIn.lightColor = dirLightColor;
  pbrIn.albedo = colorTex.Sample(texSampler, input.uv);

  float4 pbrParams = pbrParamsTex.Sample(texSampler, input.uv);
  pbrIn.metalness = pbrParams.x;
  pbrIn.roughness = pbrParams.y;  
  pbrIn.reflectivity = pbrParams.z;

  return  float4(PBR(pbrIn), 1);
}

