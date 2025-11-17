/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_BILLBOARD_OUT
{
    float3 position : POSITION;
    float2 size : SIZE;
};

struct GS_BILLBOARD_OUT
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

VS_BILLBOARD_OUT VSBillboard(uint instanceID : SV_InstanceID)
{
    VS_BILLBOARD_OUT output;
	
    output.position = gBillboard[instanceID].position;
    output.size = gBillboard[instanceID].size;
	
    return output;
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_BILLBOARD_OUT> outStream)
{
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vLook = gvCameraPosition.xyz - input[0].position;
    vLook.y = 0.0f;
    vLook = normalize(vLook);
	
    float3 vRight = cross(vUp, vLook);
    float fHalfW = input[0].size.x * 0.5f;
    float fHalfH = input[0].size.y * 0.5f;
	
    float4 pVertices[4];
    pVertices[0] = float4(input[0].position + fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[1] = float4(input[0].position + fHalfW * vRight + fHalfH * vUp, 1.0f);
    pVertices[2] = float4(input[0].position - fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[3] = float4(input[0].position - fHalfW * vRight + fHalfH * vUp, 1.0f);
	
    float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f) };

    GS_BILLBOARD_OUT output;
    for (int i = 0; i < 4; i++)
    {
        output.posW = pVertices[i].xyz;
        output.posH = mul(mul(pVertices[i], gmtxView), gmtxProjection);
        output.normalW = vLook;
        output.uv = pUVs[i];
        output.primID = primID;
        outStream.Append(output);
    }
}

float4 PSBillboard(GS_BILLBOARD_OUT input) : SV_Target
{
    float4 cTexture = gBillboardTexture.Sample(gssWrap, input.uv);
    float4 cColor = cTexture;
	
    cColor.a = cTexture.a;
	
    clip(cColor.a - 0.3f);
	
    return (cColor);
}