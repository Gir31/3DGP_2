/////////////////////////////////////////////////////////////// 
// include 
///////////////////////////////////////////////////////////////
#include "Common.hlsl"

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
// Sphere
struct VS_SPHERE_OUT
{
    matrix position : POSITION;
    float radius : RADIUS;
    uint idx : INDEX;
    bool collision : COLLISION;
};

struct GS_SPHERE_OUT
{
    float4 position : SV_POSITION;
    bool collision : COLLISION;
};

// Bounding Box
struct VS_BOUNDINGBOX_OUT
{
    float3 position : POSITION;
    float3 size : SIZE;
    uint idx : INDEX;
    bool collision : COLLISION;
};

struct GS_BOUNDINGBOX_OUT
{
    float4 position : SV_POSITION;
    bool collision : COLLISION;
};

/////////////////////////////////////////////////////////////// 
// VS - GS - PS
///////////////////////////////////////////////////////////////
// Sphere
VS_SPHERE_OUT VSSphere(uint instanceID : SV_InstanceID)
{
    VS_SPHERE_OUT output;
    SRV_SPHERE_INFO sphere = gSpheres[instanceID];
    
    output.radius = sphere.radius;
    output.idx = sphere.idx;
    output.position = gGameObjects[output.idx].gmtxGameObject;
    output.collision = sphere.collision;

    return output;
}

[maxvertexcount(192)]
void GSSphere(point VS_SPHERE_OUT input[1],
              inout LineStream<GS_SPHERE_OUT> outStream)
{
    float radius = input[0].radius;
    float4x4 mtxWorld = input[0].position;

    const int segments = 32;
    const float step = 6.2831853f / segments;
    
    matrix gmtxViewProjection = mul(gmtxView, gmtxProjection);

    // ---------------------------------------
    // XY 면 원
    // ---------------------------------------
    [unroll]
    for (int i = 0; i < segments; ++i)
    {
        float a0 = i * step;
        float a1 = (i + 1) * step;

        float3 p0 = float3(cos(a0) * radius, sin(a0) * radius, 0);
        float3 p1 = float3(cos(a1) * radius, sin(a1) * radius, 0);

        GS_SPHERE_OUT v0, v1;
        v0.position = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.position = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
        v1.collision = input[0].collision;
        outStream.Append(v0);
        outStream.Append(v1);
        outStream.RestartStrip();
    }

    // ---------------------------------------
    // XZ 면 원
    // ---------------------------------------
    [unroll]
    for (int j = 0; j < segments; ++j)
    {
        float a0 = j * step;
        float a1 = (j + 1) * step;

        float3 p0 = float3(cos(a0) * radius, 0, sin(a0) * radius);
        float3 p1 = float3(cos(a1) * radius, 0, sin(a1) * radius);

        GS_SPHERE_OUT v0, v1;
        v0.position = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.position = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
        v1.collision = input[0].collision;
        outStream.Append(v0);
        outStream.Append(v1);
        outStream.RestartStrip();
    }

    // ---------------------------------------
    // YZ 면 원
    // ---------------------------------------
    [unroll]
    for (int k = 0; k < segments; ++k)
    {
        float a0 = k * step;
        float a1 = (k + 1) * step;

        float3 p0 = float3(0, cos(a0) * radius, sin(a0) * radius);
        float3 p1 = float3(0, cos(a1) * radius, sin(a1) * radius);

        GS_SPHERE_OUT v0, v1;
        v0.position = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.position = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
        v1.collision = input[0].collision;
        outStream.Append(v0);
        outStream.Append(v1);
        outStream.RestartStrip();
    }
}

float4 PSSphere(GS_SPHERE_OUT input) : SV_Target
{
    if (input.collision)
        return float4(0.f, 1.f, 1.f, 1.f);

    return float4(1.f, 0.f, 0.f, 1.f);
}

VS_BOUNDINGBOX_OUT VSBoundingBox(uint instanceID : SV_InstanceID)
{
    VS_BOUNDINGBOX_OUT output;
    SRV_BOUNDINGBOX_INFO box = gBoundingBoxs[instanceID];
    
    output.position = box.position;
    output.size = box.size;
    output.idx = box.idx;
    output.collision = box.collision;
    
    return output;
}

// Bounding Box
[maxvertexcount(24)]
void GSBoundingBox(point VS_BOUNDINGBOX_OUT input[1],
                   inout LineStream<GS_BOUNDINGBOX_OUT> outStream)
{
    float3 position = input[0].position; // 카메라 앞 z=5 위치
    float3 size = input[0].size; // extents

    float3 corners[8] =
    {
        position + float3(-size.x, -size.y, -size.z), position + float3(size.x, -size.y, -size.z),
        position + float3(size.x, size.y, -size.z), position + float3(-size.x, size.y, -size.z),
        position + float3(-size.x, -size.y, size.z), position + float3(size.x, -size.y, size.z),
        position + float3(size.x, size.y, size.z), position + float3(-size.x, size.y, size.z)
    };
    int edges[12][2] =
    {
        { 0, 1 },
        { 1, 2 },
        { 2, 3 },
        { 3, 0 },
        { 4, 5 },
        { 5, 6 },
        { 6, 7 },
        { 7, 4 },
        { 0, 4 },
        { 1, 5 },
        { 2, 6 },
        { 3, 7 }
    };

    matrix W = gGameObjects[input[0].idx].gmtxGameObject;
    matrix VP = mul(gmtxView, gmtxProjection);

    [unroll]
    for (int i = 0; i < 12; ++i)
    {
        GS_BOUNDINGBOX_OUT a, b;
        a.position = mul(mul(float4(corners[edges[i][0]], 1), W), VP);
        a.collision = input[0].collision;
        b.position = mul(mul(float4(corners[edges[i][1]], 1), W), VP);
        b.collision = input[0].collision;
        outStream.Append(a);
        outStream.Append(b);
    }
}

float4 PSBoundingBox(GS_BOUNDINGBOX_OUT input) : SV_Target
{
    if (input.collision)
        return float4(0.f, 1.f, 0.f, 1.f);

    return float4(1.f, 0.f, 0.f, 1.f);
}