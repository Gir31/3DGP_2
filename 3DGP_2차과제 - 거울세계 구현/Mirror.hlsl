/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_MIRROR_OUT
{
    float3 position : POSITION;
    float2 size : SIZE;
};

struct GS_MIRROR_OUT
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

/////////////////////////////////////////////////////////////// 
// VS - GS - PS
///////////////////////////////////////////////////////////////

VS_MIRROR_OUT VSMirror(uint instanceID : SV_InstanceID)
{
    VS_MIRROR_OUT output;
	
    output.position = gMirror[instanceID].position;
    output.size = gMirror[instanceID].size;
	
    return output;
}

[maxvertexcount(4)]
void GSMirror(point VS_MIRROR_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_MIRROR_OUT> outStream)
{

    float3 vLook = float3(0.f, 0.f, 1.f); // Á¤¸é
    float3 vRight = float3(1.f, 0.f, 0.f);
    float3 vUp = float3(0.f, 1.f, 0.f);

    float fHalfW = input[0].size.x * 0.5f;
    float fHalfH = input[0].size.y * 0.5f;

    float4 pVertices[4];
    pVertices[0] = float4(input[0].position + fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[1] = float4(input[0].position + fHalfW * vRight + fHalfH * vUp, 1.0f);
    pVertices[2] = float4(input[0].position - fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[3] = float4(input[0].position - fHalfW * vRight + fHalfH * vUp, 1.0f);

    float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };

    GS_MIRROR_OUT output;
    for (int i = 0; i < 4; ++i)
    {
        output.posW = pVertices[i].xyz;
        output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
        output.normalW = vLook;
        output.uv = pUVs[i];
        output.primID = primID;
        outStream.Append(output);
    }
}

float4 PSMirror(GS_MIRROR_OUT input) : SV_Target
{
    float4 cColor = float4(0, 0, 0, 0);
	
    return (cColor);
}