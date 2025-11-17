#include "Common.hlsl"
#include "Light.hlsl"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;
	
    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}




float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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

VS_SPRITE_TEXTURED_OUTPUT VSTextured(VS_SPRITE_TEXTURED_INPUT input)
{
	VS_SPRITE_TEXTURED_OUTPUT output;
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


float4 PSTextured(VS_SPRITE_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTerrainTexture.Sample(gssWrap, input.uv);

	return(cColor);
}

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

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainTexture.Sample(gssWrap, input.uv0);
	float4 cDetailTexColor = gtxtDetailTexture.Sample(gssWrap, input.uv1);
	float4 cColor = cBaseTexColor * 0.5f + cDetailTexColor * 0.5f;

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//


struct VS_BILLBOARD_OUT
{
    float3 centerW : POSITION;
    float2 sizeW : SIZE;
};

struct GS_BILLBOARD_OUT
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION;
    float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    uint primID : SV_PrimitiveID;
};

VS_BILLBOARD_OUT VSBillboard(uint instanceID : SV_InstanceID)
{
    VS_BILLBOARD_OUT output;
	
    output.centerW = gBillboard[instanceID].location;
    output.sizeW = gBillboard[instanceID].size;
	
    return output;
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_BILLBOARD_OUT> outStream)
{
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vLook = gvCameraPosition.xyz - input[0].centerW;
    vLook.y = 0.0f;
    vLook = normalize(vLook);
	
    float3 vRight = cross(vUp, vLook);
    float fHalfW = input[0].sizeW.x * 0.5f;
    float fHalfH = input[0].sizeW.y * 0.5f;
	
    float4 pVertices[4];
    pVertices[0] = float4(input[0].centerW + fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[1] = float4(input[0].centerW + fHalfW * vRight + fHalfH * vUp, 1.0f);
    pVertices[2] = float4(input[0].centerW - fHalfW * vRight - fHalfH * vUp, 1.0f);
    pVertices[3] = float4(input[0].centerW - fHalfW * vRight + fHalfH * vUp, 1.0f);
	
    float2 pUVs[4] = { float2(0.f, 1.f), float2(0.f, 0.f), float2(1.f, 1.f), float2(1.f, 0.f)};

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SPHERE_OUT
{
    matrix center : POSITION;
    float radius : RADIUS;
    uint objectIndex : INDEX;
    bool collision : COLLISION;
};

struct GS_SPHERE_OUT
{
    bool collision : COLLISION;
    float4 posH : SV_POSITION;
};

VS_SPHERE_OUT VSSphere(uint instanceID : SV_InstanceID)
{
    VS_SPHERE_OUT output;
    SRV_SPHERE_INFO sphere = gSpheres[instanceID];
    
    output.radius = sphere.radius;
    output.objectIndex = sphere.objectIndex;
    output.center = gGameObjects[output.objectIndex].gmtxGameObject;
    output.collision = sphere.collision;

    return output;
}

[maxvertexcount(192)]
void GSSphere(point VS_SPHERE_OUT input[1],
              inout LineStream<GS_SPHERE_OUT> outStream)
{
    float radius = input[0].radius;
    float4x4 mtxWorld = input[0].center;

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
        v0.posH = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.posH = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
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
        v0.posH = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.posH = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
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
        v0.posH = mul(mul(float4(p0, 1.0f), mtxWorld), gmtxViewProjection);
        v0.collision = input[0].collision;
        v1.posH = mul(mul(float4(p1, 1.0f), mtxWorld), gmtxViewProjection);
        v1.collision = input[0].collision;
        outStream.Append(v0);
        outStream.Append(v1);
        outStream.RestartStrip();
    }
}

float4 PSSphere(GS_SPHERE_OUT input) : SV_Target
{
    if(input.collision)
        return float4(0.f, 1.f, 1.f, 1.f);

    return float4(1.f, 0.f, 0.f, 1.f);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_BOUNDINGBOX_OUT
{
    float3 center : POSITION;
    float3 size : SIZE;
    uint objectIndex : INDEX;
    bool collision : COLLISION;
};

struct GS_BOUNDINGBOX_OUT
{
    float4 posH : SV_POSITION;
    bool collision : COLLISION;
};

VS_BOUNDINGBOX_OUT VSBoundingBox(uint instanceID : SV_InstanceID)
{
    VS_BOUNDINGBOX_OUT output;
    SRV_BOUNDINGBOX_INFO box = gBoundingBoxs[instanceID];
    
    output.center = box.center;
    output.size = box.size;
    output.objectIndex = box.objectIndex;
    output.collision = box.collision;
    
    return output;
}

[maxvertexcount(24)]
void GSBoundingBox(point VS_BOUNDINGBOX_OUT input[1],
                   inout LineStream<GS_BOUNDINGBOX_OUT> outStream)
{
    float3 center = input[0].center; // 카메라 앞 z=5 위치
    float3 size = input[0].size; // extents

    float3 corners[8] =
    {
        center + float3(-size.x, -size.y, -size.z), center + float3(size.x, -size.y, -size.z),
        center + float3(size.x, size.y, -size.z), center + float3(-size.x, size.y, -size.z),
        center + float3(-size.x, -size.y, size.z), center + float3(size.x, -size.y, size.z),
        center + float3(size.x, size.y, size.z), center + float3(-size.x, size.y, size.z)
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

    matrix W = gGameObjects[input[0].objectIndex].gmtxGameObject;
    matrix VP = mul(gmtxView, gmtxProjection);

    [unroll]
    for (int i = 0; i < 12; ++i)
    {
        GS_BOUNDINGBOX_OUT a, b;
        a.posH = mul(mul(float4(corners[edges[i][0]], 1), W), VP);
        a.collision = input[0].collision;
        b.posH = mul(mul(float4(corners[edges[i][1]], 1), W), VP);
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//



struct VS_UI_OUT
{
    float3 center : POSITION;
    float2 size : SIZE;
    uint textureMask : TEXTUREMASK;
};

struct GS_UI_OUT
{
    float4 posH : SV_POSITION;
    float2 uv : TEXCOORD;
    uint mask : TEXMASK;
};

VS_UI_OUT VSUI(uint instanceID : SV_InstanceID)
{
    VS_UI_OUT output;
    SRV_UI_INFO ui = gUI[instanceID];

    output.center = ui.location;
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
    float3 c = input[0].center;

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
        o.posH = float4(v[index[j]], 1);
        o.uv = uv[index[j]];
        o.mask = mask;

        outStream.Append(o);

        if (j % 3 == 2)
            outStream.RestartStrip();
    }
}

float4 PSUI(GS_UI_OUT input) : SV_Target
{
    float4 tex;
    if(input.mask == 0)
        tex = gUIHPTextures.Sample(gSampler, input.uv);
    else if (input.mask == 1)
        tex = gUICoordinateTextures.Sample(gSampler, input.uv);
    
    if (tex.r > 0.8 && tex.g > 0.8 && tex.b > 0.8)
    {
        tex.a = 0.0;
    }

    return tex;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_MIRRORMASK_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_MIRRORMASK_OUTPUT
{
    float4 posH : SV_POSITION;
};

VS_MIRRORMASK_OUTPUT VSMirrorMask(VS_MIRRORMASK_INPUT input)
{
    VS_MIRRORMASK_OUTPUT output;

    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    float4 worldPos = mul(float4(input.pos, 1.0f), mtxGameObject);
    float4 viewPos = mul(worldPos, gmtxView);
    output.posH = mul(viewPos, gmtxProjection);

    return output;
}

float4 PSMirrorMask() : SV_Target
{
    return float4(0, 0, 0, 0); // Writemask=0이기에 출력은 무시됨
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_MIRROR_SCENE_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

struct VS_MIRROR_SCENE_OUTPUT
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

VS_MIRROR_SCENE_OUTPUT VSMirrorScene(VS_MIRROR_SCENE_INPUT input)
{
    VS_MIRROR_SCENE_OUTPUT output;

    // 오브젝트 월드 행렬은 기존과 동일하게 SRV에서 가져옴
    matrix mtxGameObject = gGameObjects[index].gmtxGameObject;

    // World 좌표
    output.positionW = (float3) mul(float4(input.position, 1.0f), mtxGameObject);

    // World 기준 노말/탄젠트/TBN
    output.normalW = mul(input.normal, (float3x3) mtxGameObject);
    output.tangentW = (float3) mul(float4(input.tangent, 1.0f), mtxGameObject);
    output.bitangentW = (float3) mul(float4(input.bitangent, 1.0f), mtxGameObject);

    // 반사 카메라 View/Proj 사용
    float4 viewPos = mul(float4(output.positionW, 1.0f), gmtxReflectView_mirror);
    output.position = mul(viewPos, gmtxProjection_mirror);

    output.uv = input.uv;
    output.textureMask = gGameObjects[index].gnTexturesMask;
    output.mat = gGameObjects[index].gMaterial;

    return output;
}

float4 PSMirrorScene(VS_MIRROR_SCENE_OUTPUT input) : SV_TARGET
{
    float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cNormalColor = float4(0.5f, 0.5f, 1.0f, 1.0f); // 기본 노말 (0,0,1)
    float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // 텍스처 샘플링 (기존 PSStandard 그대로)
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

    float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
    float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);

    // 노말맵이 있는 경우 TBN 적용
    if (input.textureMask & MATERIAL_NORMAL_MAP)
    {
        float3 N = normalize(input.normalW);
        float3 T = normalize(input.tangentW);
        float3 B = normalize(input.bitangentW);

        float3x3 TBN = float3x3(T, B, N);

        float3 vNormalTex = normalize(cNormalColor.rgb * 2.0f - 1.0f); // [0,1] -> [-1,1]
        float3 normalW = normalize(mul(vNormalTex, TBN));

        // 조명 계산 (Lighting은 Light.hlsl에 정의된 기존 함수)
        cIllumination = Lighting(input.positionW, normalW, input.mat);

        // 기본 색과 조명을 섞기 (가중치는 취향)
        cColor = lerp(cColor, cIllumination, 0.5f);
    }
    else
    {
        // 노말맵이 없으면 기본 normalW 사용
        float3 normalW = normalize(input.normalW);
        cIllumination = Lighting(input.positionW, normalW, input.mat);
        cColor = lerp(cColor, cIllumination, 0.5f);
    }

    return cColor;
}
