#include "Common.hlsli"

struct PBRinput {
  float4 worldPos;
  float4 normal;
  float4 viewDir;
  float4 halfVector;
  float4 lightDir;
  float4 lightColor;
  float4 albedo;
  float4 emissive;
};

static float roughness = 0.1f;

// Normal distribution function
float3 _D(PBRinput input) {
  static float roughnessSqr = roughness * roughness;
  float denominator = _PI * pow(pow(dot(input.normal, input.halfVector), 2) * (roughnessSqr - 1) + 1, 2);
  return roughnessSqr / max(denominator, 0.00001);
}

// Geometry shadowing function
float _G1(float3 N, float3 X) {
  static float k = roughness * 0.5;
  float denominator = dot(N, X) * (1 - k) + k;
  return dot(N, X) / max(denominator, 0.00001);
}
float3 _G(PBRinput input) {
  return _G1(input.normal, input.lightDir) * _G1(input.normal, input.viewDir);
}

// Fresnell function
float _F(PBRinput input) {
  static float f0 = 0.2;
  return f0 + (1 - f0) * pow(1 - dot(input.viewDir, input.halfVector), 5);
}

// PBR lighting
float3 PBR(PBRinput input) {
  float kd = 0.8;

  // Lambert diffuse
  float3 fd = input.albedo / _PI;

  // Cook-Torrance specular
  float4 fsDenom = (4 * dot(input.viewDir, input.normal) * dot(input.lightDir, input.normal));
  float3 fs = _D(input) * _G(input) * _F(input) / fsDenom.rgb;

  // Incoming light
  float3 li = input.lightColor;// * CALC_SHADOW(input.worldPos);

  // BRDF
  float3 fr = max(kd * fd + fs, float3(0, 0, 0));

  // Cosine Law
  float lDotN = dot(input.lightDir, input.normal);

  return input.emissive + /* SUM -> */ (fr * li * lDotN);

}
