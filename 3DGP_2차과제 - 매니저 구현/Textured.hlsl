/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_SPRITE_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_SPRITE_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

/////////////////////////////////////////////////////////////// 
// VS - PS
///////////////////////////////////////////////////////////////
VS_SPRITE_TEXTURED_OUTPUT VSTextured(VS_SPRITE_TEXTURED_INPUT input)
{
    VS_SPRITE_TEXTURED_OUTPUT output;
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

float4 PSTextured(VS_SPRITE_TEXTURED_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTerrainTexture.Sample(gssWrap, input.uv);

    return (cColor);
}