/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct VS_UI_OUT
{
    float3 position : POSITION;
    float2 size : SIZE;
    uint textureMask : TEXTUREMASK;
};

struct GS_UI_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    uint textureMask : TEXTUREMASK;
};

/////////////////////////////////////////////////////////////// 
// VS - GS - PS
///////////////////////////////////////////////////////////////
VS_UI_OUT VSUI(uint instanceID : SV_InstanceID)
{
    VS_UI_OUT output;
    SRV_UI_INFO ui = gUI[instanceID];

    output.position = ui.position;
    output.size = ui.size;
    output.textureMask = ui.uiTexturesMask;

    return output;
}

[maxvertexcount(6)]
void GSUI(
    point VS_UI_OUT input[1],
    inout TriangleStream<GS_UI_OUT> outStream)
{
    float2 halfSize = input[0].size * 0.5f;
    float3 c = input[0].position;

    float3 v[4] =
    {
        float3(-halfSize.x, -halfSize.y, 0),
        float3(-halfSize.x, halfSize.y, 0),
        float3(halfSize.x, -halfSize.y, 0),
        float3(halfSize.x, halfSize.y, 0)
    };

    uint mask = input[0].textureMask;

    if (mask == 0)
    {
        float3 forward = normalize(-gmtxView._31_32_33);
        float yaw = atan2(forward.x, forward.z);

        float s = sin(yaw);
        float ccos = cos(yaw);

        for (int i = 0; i < 4; i++)
        {
            float x = v[i].x;
            float y = v[i].y;

            float rx = x * ccos - y * s;
            float ry = x * s + y * ccos;

            v[i] = float3(rx, ry, 0) + c;
        }
    }
    else
    {
        // 고정 UI → 회전 없이 그냥 위치 이동만
        for (int i = 0; i < 4; i++)
        {
            v[i] += c;
        }
    }

    float2 uv[4] =
    {
        float2(0, 1),
        float2(0, 0),
        float2(1, 1),
        float2(1, 0)
    };

    uint index[6] = { 0, 1, 2, 2, 1, 3 };

    [unroll]
    for (int j = 0; j < 6; j++)
    {
        GS_UI_OUT o;
        o.position = float4(v[index[j]], 1);
        o.uv = uv[index[j]];
        o.textureMask = mask;

        outStream.Append(o);

        if (j % 3 == 2)
            outStream.RestartStrip();
    }
}

float4 PSUI(GS_UI_OUT input) : SV_Target
{
    float4 tex;
    if (input.textureMask == 0)
        tex = gUIHPTextures.Sample(gSampler, input.uv);
    else if (input.textureMask == 1)
        tex = gUICoordinateTextures.Sample(gSampler, input.uv);
    
    if (tex.r > 0.8 && tex.g > 0.8 && tex.b > 0.8)
    {
        tex.a = 0.0;
    }

    return tex;
}