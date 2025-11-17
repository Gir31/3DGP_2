/////////////////////////////////////////////////////////////// 
// define
///////////////////////////////////////////////////////////////
#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

/////////////////////////////////////////////////////////////// 
// struct
///////////////////////////////////////////////////////////////
struct MATERIAL
{
    float4 m_cAmbient;
    float4 m_cDiffuse;
    float4 m_cSpecular; //a = power
    float4 m_cEmissive;
};

struct SRV_GAMEOBJECT_INFO
{
    matrix gmtxGameObject;
    MATERIAL gMaterial;
    uint gnTexturesMask;
};

struct SRV_BILLBOARD_INFO
{
    float3 location;
    float2 size;
};

struct SRV_SPHERE_INFO
{
    float radius;
    uint objectIndex;
    bool collision;
};

struct SRV_BOUNDINGBOX_INFO
{
    float3 center;
    float3 size;
    uint objectIndex;
    bool collision;
};

struct SRV_UI_INFO\

{
    float3 location;
    float2 size;
    uint uiTexturesMask;
};

/////////////////////////////////////////////////////////////// 
// cbuffer
///////////////////////////////////////////////////////////////
cbuffer cbCameraInfo : register(b1)
{
    matrix gmtxView : packoffset(c0);
    matrix gmtxProjection : packoffset(c4);
    float3 gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectIndex : register(b3)
{
    uint index : packoffset(c0);
};

cbuffer cbScene : register(b5)
{
    matrix gmtxReflectView_mirror : packoffset(c0);
    matrix gmtxProjection_mirror : packoffset(c4);
    float3 gvCameraPosition_mirror : packoffset(c8);
}

/////////////////////////////////////////////////////////////// 
// StructuredBuffer
///////////////////////////////////////////////////////////////
StructuredBuffer<SRV_GAMEOBJECT_INFO> gGameObjects : register(t18);
StructuredBuffer<SRV_BOUNDINGBOX_INFO> gBoundingBoxs : register(t19);
StructuredBuffer<SRV_SPHERE_INFO> gSpheres : register(t20);
StructuredBuffer<SRV_BILLBOARD_INFO> gBillboard : register(t21);
StructuredBuffer<SRV_UI_INFO> gUI : register(t22);

/////////////////////////////////////////////////////////////// 
// Texture2D
///////////////////////////////////////////////////////////////
Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);
TextureCube gtxtSkyCubeTexture : register(t13);
Texture2D gtxtTerrainTexture : register(t14);
Texture2D gtxtDetailTexture : register(t15);
Texture2D gtxtAlphaTexture : register(t16);
Texture2D gBillboardTexture : register(t17);
Texture2D gUIHPTextures : register(t23); // UI용 텍스처 배열
Texture2D gUICoordinateTextures : register(t24); // UI용 텍스처 배열
Texture2D gUICoordinate2Textures : register(t25); // UI용 텍스처 배열

/////////////////////////////////////////////////////////////// 
// SmaplerState
///////////////////////////////////////////////////////////////
SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);
SamplerState gSampler : register(s2);