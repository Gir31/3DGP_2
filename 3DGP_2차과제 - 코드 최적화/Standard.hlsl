/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"
#include "Light.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_STANDARD_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
    float3 tangentW : TANGENT;
    float3 bitangentW : BITANGENT;
    float2 uv : TEXCOORD;
    uint textureMask : TEXTUREMASK;
    MATERIAL mat : MATERIAL;
};

/////////////////////////////////////////////////////////////// 
// VS - PS
///////////////////////////////////////////////////////////////
VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
    VS_STANDARD_OUTPUT output;

    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    output.positionW = (float3) mul(float4(input.position, 1.0f), mtxGameObject);
    output.normalW = mul(input.normal, (float3x3) mtxGameObject);
    output.tangentW = (float3) mul(float4(input.tangent, 1.0f), mtxGameObject);
    output.bitangentW = (float3) mul(float4(input.bitangent, 1.0f), mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.textureMask = gGameObjects[index].gnTexturesMask;
    output.mat = gGameObjects[index].gMaterial;

    return (output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    if (input.textureMask & MATERIAL_ALBEDO_MAP)
        cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
    if (input.textureMask & MATERIAL_SPECULAR_MAP)
        cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
    if (input.textureMask & MATERIAL_NORMAL_MAP)
        cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
    if (input.textureMask & MATERIAL_METALLIC_MAP)
        cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
    if (input.textureMask & MATERIAL_EMISSION_MAP)
        cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

    float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
    if (input.textureMask & MATERIAL_NORMAL_MAP)
    {
        float3 normalW = input.normalW;
        float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
        float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] ¡æ [-1, 1]
        normalW = normalize(mul(vNormal, TBN));
        cIllumination = Lighting(input.positionW, normalW, input.mat);
        cColor = lerp(cColor, cIllumination, 0.5f);
    }

    return (cColor);
}