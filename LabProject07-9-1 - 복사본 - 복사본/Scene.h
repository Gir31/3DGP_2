//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Shader.h"
#include "Player.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

struct LIGHT
{
	XMFLOAT4				m_xmf4Ambient;
	XMFLOAT4				m_xmf4Diffuse;
	XMFLOAT4				m_xmf4Specular;
	XMFLOAT3				m_xmf3Position;
	float 					m_fFalloff;
	XMFLOAT3				m_xmf3Direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
	int						m_nLights;
};

struct VS_VB_INSTANCE
{
	XMFLOAT4X4 m_xmf4x4World;
	MATERIAL material;
};

class CScene
{
public:
    CScene();
    ~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();

	void UpdateInstanceShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	void FillInstanceRecursive(CGameObject* pObject, int& index);

	virtual void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual void ReleaseObjects();

	bool ProcessInput(UCHAR *pKeysBuffer);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL);

	virtual void ReleaseUploadBuffers();

	virtual void Reset() {}

	CPlayer						*m_pPlayer = NULL;

public:
	//ID3D12RootSignature			*m_pd3dGraphicsRootSignature = NULL;

	CGameObject					**m_ppGameObjects = NULL;
	int							m_nGameObjects = 0;

	/////////////////////////////////////////////////////////////////////////
	// INSTANCE
	CGameObject					*inst_ppGameObjects = NULL;
	int							inst_nGameObjects = 0;

	ID3D12Resource* m_pd3dcbGameObjects = nullptr;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = nullptr;
	/////////////////////////////////////////////////////////////////////////

	LIGHT						*m_pLights = NULL;
	int							m_nLights = 0;

	XMFLOAT4					m_xmf4GlobalAmbient;

	ID3D12Resource				*m_pd3dcbLights = NULL;
	LIGHTS						*m_pcbMappedLights = NULL;

	float						m_fElapsedTime = 0.0f;

public:
	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;
	ID3D12DescriptorHeap* m_pd3dSrvDescriptorHeap = NULL;

	void AssignCbvRecursive(CGameObject* pObject,
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		D3D12_CPU_DESCRIPTOR_HANDLE& d3dCPUHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE& d3dGPUHandle,
		UINT nIncrementSize);

	D3D12_GPU_DESCRIPTOR_HANDLE m_d3dInstancingSrvGpuHandle = { 0 };

	std::vector< VS_VB_INSTANCE> Instance_data;
};
