#pragma once
#include "CStandardShader.h"

class CObjectsShader : public CStandardShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);


	int GetNumberOfObjects() { return(m_nGameObjects); }

protected:

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	SRV_GAMEOBJECT_INFO* m_pcbMappedGameObjects = NULL;

	float							m_fxPitch = 0;
	float							m_fyPitch = 0;
	float							m_fzPitch = 0;
	int								m_xObjects = 0;
	int								m_yObjects = 0;
	int								m_zObjects = 0;
};

class CEnemyShader : public CObjectsShader
{
public:
	CEnemyShader();
	virtual ~CEnemyShader();

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext = NULL);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState = 0);
};