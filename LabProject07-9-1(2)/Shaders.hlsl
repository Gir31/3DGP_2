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

cbuffer cbGameObjectInfo : register(b2)
{
	matrix					gmtxGameObject : packoffset(c0);
	MATERIAL				gMaterial : packoffset(c4);
};

struct INSTANCEDGAMEOBJECTINFO
{
    matrix m_mtxGameObject;
    MATERIAL m_gMaterial;
};

StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);

#include "Light.hlsl"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
	float4 color : COLOR;
#endif
};

VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
	output.normalW = normalize(output.normalW);
	output.color = Lighting(output.positionW, output.normalW);
#endif
	return(output);
}

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
	return(input.color);
#else
	input.normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, input.normalW);

	return(color);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//INSTANCING

VS_LIGHTING_OUTPUT VSInstancingLighting(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_LIGHTING_OUTPUT output;
	
    matrix mtxWorld	= gGameObjectInfos[nInstanceID].m_mtxGameObject;
    MATERIAL material = gGameObjectInfos[nInstanceID].m_gMaterial;

    output.normalW = mul(input.normal, (float3x3) mtxWorld);
    output.positionW = (float3) mul(float4(input.position, 1.0f), mtxWorld);
	
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color   = Lighting(output.positionW, output.normalW, material);
#endif

    return (output);
}

float4 PSInstancingLighting(VS_LIGHTING_OUTPUT input, uint nInstanceID : SV_InstanceID) : SV_TARGET
{
    MATERIAL material = gGameObjectInfos[nInstanceID].m_gMaterial;

#ifdef _WITH_VERTEX_LIGHTING
    return input.color;
#else
    input.normalW = normalize(input.normalW);
    float4 color = Inst_ComputeLighting(input.positionW, input.normalW, material);
	
    return (color);
#endif
}