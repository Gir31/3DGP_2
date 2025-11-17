/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_TERRAIN_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

/////////////////////////////////////////////////////////////// 
// VS - PS
///////////////////////////////////////////////////////////////
VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_TERRAIN_OUTPUT output;
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);
    output.color = input.color;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return (output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainTexture.Sample(gssWrap, input.uv0);
    float4 cDetailTexColor = gtxtDetailTexture.Sample(gssWrap, input.uv1);
    float4 cColor = cBaseTexColor * 0.5f + cDetailTexColor * 0.5f;

    return (cColor);
}