cbuffer Constant : register(b1){
    matrix viewproj;
    matrix view;
    matrix proj;
};

struct VSOutput
{
    float3 TexCoord : TEXCOORD;
    float4 pos : SV_POSITION;
};

VSOutput main(float3 pos : POSITION)
{
    VSOutput output;

    // Rotate into view-space, centered on the camera
    float3 positionVS = mul(float4(pos, 0.0), view).xyz;

    // Transform to clip-space
    output.pos = mul(float4(positionVS, 1.0f), proj);
    output.pos = output.pos.xyzz;

    // Make a texture coordinate
    output.TexCoord = pos.xyz;

    return output;
}