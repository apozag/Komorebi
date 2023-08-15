#include "Lighting.hlsli"

struct PBRinput {
  float4 worldPos;
  float4 normal;
  float4 viewDir;
  float4 halfVector;
  float4 lightDir;
  float4 lightColor;
  float4 albedo;
  float4 emissive;
  float roughness;
  float metalness;
};

Texture2D	BrdfLutTex : register(t14);
SamplerState brdfSampler;

// Normal distribution function
float _D(PBRinput input) {
  float alpha = input.roughness * input.roughness;
  float alphaSqr = alpha * alpha;
  float cosLh = max(dot(input.normal, input.halfVector), 0);
  float denom = cosLh * cosLh * (alphaSqr - 1) + 1;
  return alphaSqr / (_PI * denom * denom);//max(denominator, 0.00001);
}

// Geometry shadowing function
float _G1(float3 N, float3 X, float roughness) {
  //float k = roughness * 0.5;
  float r = roughness + 1.0;
  float k = (r * r) / 0.8;
  float denominator = max(dot(N, X), 0) * (1 - k) + k;
  return max(dot(N, X), 0) / denominator;//max(denominator, 0.00001);
}
float _G(PBRinput input) {
  return _G1(input.normal, input.lightDir, input.roughness) * _G1(input.normal, input.viewDir, input.roughness);
}

// Fresnell function
float3 _F(PBRinput input) {
  float f0 = 0.4;//input.metalness;
  float3 vF0 = input.albedo * lerp(f0, 1.0, input.metalness);
  float LdotH = max(dot(input.viewDir, input.halfVector), 0);
  return vF0 + (float3(1,1,1) - vF0) * pow(1 - LdotH, 5);
}

// PBR lighting
float3 PBR(PBRinput input) {

  // Lambert diffuse
  float3 fd = input.albedo / _PI;

  // Cook-Torrance specular
  float fsDenom = (4 * max(dot(input.viewDir, input.normal), 0) * max(dot(input.lightDir, input.normal), 0));
  float3 fresnell = _F(input);
  float3 fs = _D(input) * _G(input) * fresnell / max(fsDenom, 0.00001); 

  // Incoming light
  float3 li = input.lightColor * CALC_SHADOW(input.worldPos);

  // BRDF
  //float kd = lerp(1 - fresnell, 0, input.metalness);
  float3 kd = float3(1,1,1) - fresnell;
  float3 fr = max(kd * fd + fs, float3(0, 0, 0));

  // Cosine Law
  float lDotN = max(dot(input.lightDir, input.normal), 0);

  return fr * li * lDotN;

}

float3 PBRDiffuseIrradiance(PBRinput input) {  
  float3 kS = _F(input);
  float3 kd = float3(1,1,1) - kS;
  return kd * input.lightColor * input.albedo;
}

float3 PBRSpecularIBL(PBRinput input) {
  float3 kS = _F(input);
  float NdotV = dot(input.normal, input.viewDir);
  float2 envBRDF = BrdfLutTex.Sample(brdfSampler, float2(NdotV, input.roughness)).xy;
  return (kS * envBRDF.x + envBRDF.y) * input.lightColor;
}
