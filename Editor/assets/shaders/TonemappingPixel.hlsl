#include "Common.hlsli"

cbuffer params : register(b7) {
  float whitePoint = 1;
  float avgLuminance = 1;
};

SamplerState texSampler : register(s0);
Texture2D colorTex : register(t0);

float Log2Exposure(in float avgLuminance) {
  float KeyValue = 1.0;
  avgLuminance = max(avgLuminance, 0.00001f);
  float linearExposure = (KeyValue / avgLuminance);
  float exposure = log2(max(linearExposure, 0.00001f));
  return exposure;
}


// Determines the color based on exposure settings
float3 CalcExposedColor(in float3 color, in float avgLuminance, in float offset, out float exposure) {
  exposure = Log2Exposure(avgLuminance);
  exposure += offset;
  return exp2(exposure) * color;
}

float3 LinearTosRGB(in float3 color) {
  float3 x = color * 12.92f;
  float3 y = 1.055f * pow(saturate(color), 1.0f / 2.4f) - 0.055f;

  float3 clr = color;
  clr.r = color.r < 0.0031308f ? x.r : y.r;
  clr.g = color.g < 0.0031308f ? x.g : y.g;
  clr.b = color.b < 0.0031308f ? x.b : y.b;

  return clr;
}

// Applies exposure and tone mapping to the specific color, and applies
// the threshold to the exposure value.
float3 ToneMap(in float3 color, in float avgLuminance, in float threshold, out float exposure) {
  color = CalcExposedColor(color, avgLuminance, threshold, exposure);

  float3 output = 0;
  output = LinearTosRGB(color);

  return output;
}

float4 main(VSout input) : SV_TARGET {
  float3 color = colorTex.Sample(texSampler, input.uv).rgb;
  float exposure = whitePoint - whitePoint;
  color = ToneMap(color, avgLuminance, 0, exposure);

  return float4(color, 1.0f);
}
