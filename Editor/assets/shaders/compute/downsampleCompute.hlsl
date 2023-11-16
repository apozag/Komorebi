cbuffer Params : register(b0){
  /*float texelSizeX;
  float texelSizeY;
	float mipLevel;*/
	float2 srcSize;
	float2 dstSize;
};

//SamplerState texSampler : register(s0);
//Texture2D tex : register(t0);

RWTexture2D<float4> srcUAV : register(u0);
RWTexture2D<float4> dstUAV : register(u1);

//float4 Sample(float2 uv) {
//	return tex.SampleLevel(texSampler, uv, mipLevel);
//}

//float4 SampleBox(float2 uv) {
//	float2 o = float2(-0.5 * texelSizeX, 0.5 * texelSizeY);
//	float4 s = Sample(uv + o.xx) + Sample(uv + o.xy) +
//		Sample(uv + o.yx) + Sample(uv + o.yy);
//	return s * 0.25;
//}

float4 SampleBox(float2 uv) {
	float2 o = float2(0, 1);
	float4 s = srcUAV[uv + o.xx] + srcUAV[uv + o.xy] +
		srcUAV[uv + o.yx] + srcUAV[uv + o.yy];
	return s * 0.25;
}

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	float2 dst2src = srcSize/dstSize;
	dstUAV[DTid.xy] = SampleBox(DTid.xy * dst2src);
}