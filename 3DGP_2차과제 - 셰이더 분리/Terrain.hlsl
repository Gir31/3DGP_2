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

struct VS_OUTPUT
{
    float3 position : POSITION; 
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct HS_CONSTANT_DATA_OUTPUT
{
    float EdgeTessFactor[3] : SV_TessFactor;
    float InsideTessFactor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

struct DS_OUTPUT
{
    float4 position : SV_POSITION; 
    float4 color : COLOR;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

/////////////////////////////////////////////////////////////// 
// VS - HS - DS - PS
///////////////////////////////////////////////////////////////
VS_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = input.position;
    output.color = input.color;
    output.uv0 = input.uv0;
    output.uv1 = input.uv1;

    return output;
}

HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(InputPatch<VS_OUTPUT, 3> ip, uint PatchID : SV_PrimitiveID)
{
    HS_CONSTANT_DATA_OUTPUT Output;

    float3 center = (ip[0].position + ip[1].position + ip[2].position) / 3.0f;
    float d = distance(center, gvCameraPosition);

    float d_min = 100.0f;
    float d_max = 500.0f;
    float minTess = 1.0f;
    float maxTess = 64.0f;

    float s = saturate((d - d_min) / (d_max - d_min));

    float tessFactor = lerp(maxTess, minTess, s);

    Output.EdgeTessFactor[0] = tessFactor;
    Output.EdgeTessFactor[1] = tessFactor;
    Output.EdgeTessFactor[2] = tessFactor;
    Output.InsideTessFactor = tessFactor;

    return Output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_OUTPUT HSTerrain(InputPatch<VS_OUTPUT, 3> p, uint i : SV_OutputControlPointID, uint PatchID : SV_PrimitiveID)
{
    HS_OUTPUT Output;
    Output.position = p[i].position;
    Output.color = p[i].color;
    Output.uv0 = p[i].uv0;
    Output.uv1 = p[i].uv1;
    return Output;
}

[domain("tri")]
DS_OUTPUT DSTerrain(HS_CONSTANT_DATA_OUTPUT input, float3 domain : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
    DS_OUTPUT Output;

    float3 position = patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z;
    float2 uv0 = patch[0].uv0 * domain.x + patch[1].uv0 * domain.y + patch[2].uv0 * domain.z;
    float2 uv1 = patch[0].uv1 * domain.x + patch[1].uv1 * domain.y + patch[2].uv1 * domain.z;
    float4 color = patch[0].color * domain.x + patch[1].color * domain.y + patch[2].color * domain.z;

    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;
    Output.position = mul(mul(mul(float4(position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);

    Output.color = color;
    Output.uv0 = uv0;
    Output.uv1 = uv1;

    return Output;
}

float4 PSTerrain(DS_OUTPUT input) : SV_TARGET
{
    float4 cBaseTexColor = gtxtTerrainTexture.Sample(gssWrap, input.uv0);
    float4 cDetailTexColor = gtxtDetailTexture.Sample(gssWrap, input.uv1);
    float4 cColor = cBaseTexColor * 0.5f + cDetailTexColor * 0.5f;

    return cColor;
}