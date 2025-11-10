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

class CDescriptorHeap
{
public:
	CDescriptorHeap();
	~CDescriptorHeap();

	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;
public:
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dGPUDescriptorHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCPUObjectDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dGPUObjectDescriptorHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() { return(m_d3dCPUDescriptorHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() { return(m_d3dGPUDescriptorHandle); }
};

class CScene
{
public:
	CScene() {}
    ~CScene() {}

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList) {}
	virtual void ReleaseShaderVariables() {}

	virtual void BuildDefaultLightsAndMaterials() {}
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL) {}
	virtual void ReleaseObjects() {}

	bool ProcessInput(UCHAR *pKeysBuffer) { return(false); }
	virtual void AnimateObjects(float fTimeElapsed) {}
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera=NULL, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL) {}

	virtual void ReleaseUploadBuffers(){}

	CPlayer								*m_pPlayer = NULL;

public:
	int									m_nShaders = 0;
	CShader								**m_ppShaders = NULL;

	LIGHT								*m_pLights = NULL;
	int									m_nLights = 0;

	XMFLOAT4							m_xmf4GlobalAmbient;

	ID3D12Resource						*m_pd3dcbLights = NULL;
	LIGHTS								*m_pcbMappedLights = NULL;

public:
	ID3D12Resource						*m_pd3dcbGameObjects = NULL;
	std::vector<SRV_GAMEOBJECT_INFO>	m_vGameObjectsInfo;
	UINT								SRVIndex = 0; 

	void AddGameObjectInfo(CGameObject* gameObject, XMFLOAT4X4* parentMatrix); 
public:
	static CDescriptorHeap*				m_pDescriptorHeap;

	static void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);
	void CreateShaderResourceView(ID3D12Device* pd3dDevice);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }
};
