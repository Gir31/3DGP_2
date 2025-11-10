struct MATERIAL
{
	float4					m_cAmbient;
	float4					m_cDiffuse;
	float4					m_cSpecular; //a = power
	float4					m_cEmissive;
};

cbuffer cbCameraInfo : register(b1)
{
	matrix					gmtxView : packoffset(c0);
	matrix					gmtxProjection : packoffset(c4);
	float3					gvCameraPosition : packoffset(c8);
};

//cbuffer cbGameObjectInfo : register(b2)
//{
//	matrix					gmtxGameObject : packoffset(c0);
//	MATERIAL				gMaterial : packoffset(c4);
//	uint					gnTexturesMask : packoffset(c8);
//};

struct SRV_GAMEOBJECT_INFO
{
    matrix gmtxGameObject;
    MATERIAL gMaterial;
    uint gnTexturesMask;
};

cbuffer cbGameObjectIndex : register(b3)
{
    uint index : packoffset(c0);
};

StructuredBuffer<SRV_GAMEOBJECT_INFO> gGameObjects : register(t18);

#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);

SamplerState gssWrap : register(s0);

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

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (input.textureMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	if (input.textureMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	if (input.textureMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	if (input.textureMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	if (input.textureMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);

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

	return(cColor);
}

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
	
    matrix viewNoTrans = gmtxView;
    viewNoTrans._41 = 0.0f;
    viewNoTrans._42 = 0.0f;
    viewNoTrans._43 = 0.0f;
	
    output.position = mul(mul(mul(float4(input.position, 1.0f), mtxGameObject), viewNoTrans), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

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
Texture2D gtxtTerrainTexture : register(t14);
Texture2D gtxtDetailTexture : register(t15);
Texture2D gtxtAlphaTexture : register(t16);

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
Texture2D gBillboardTexture : register(t17);

struct VS_BILLBOARD_IN
{
    float3 posW : POSITION;
    float2 sizeW : SIZE;
};

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

VS_BILLBOARD_OUT VSBillboard(VS_BILLBOARD_IN input)
{
    VS_BILLBOARD_OUT output;
	
    output.centerW = input.posW;
    output.sizeW = input.sizeW;
	
    return output;
}

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_BILLBOARD_OUT> outStream)
{
    float3 vUp = float3(0.f, 1.f, 0.f);
    float3 vLook = gvCameraPosition.xyz - input[0].centerW;
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