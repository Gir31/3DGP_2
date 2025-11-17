//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "DescriptorHeap.h"
#include "Shader.h"
#include "Player.h"
#include "CollisionManager.h"

#define MAX_LIGHTS			16 

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

//#define FULL_INCLUSION_TEST

enum {
	SKY_BOX_SHADER,
	TERRAIN_SHADER,
	ENEMY_SHADER,
	BILLBOARD_SHADER,
	UI_SHADER
};

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

class CScene
{
public:
	CScene() { }
	~CScene() { }

public:
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) { return false; }

	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);

	void BuildDefaultLightsAndMaterials();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL) {}

	bool ProcessInput(UCHAR* pKeysBuffer) { return(false); }
	void AnimateObjects(float fTimeElapsed, CCamera* pCamera = NULL);
	void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL);

	void ReleaseObjects();
	void ReleaseUploadBuffers();
	void ReleaseShaderVariables();

public:
	ID3D12Resource* m_pd3dcbLights = NULL;
	ID3D12Resource* m_pd3dGameObjects[2] = { nullptr, nullptr };
	ID3D12Resource* m_pd3dUploadBuffer[2] = { nullptr, nullptr };
	ID3D12Resource* m_pd3dBillboards;
	ID3D12Resource* m_pd3UIs;

	LIGHTS* m_pcbMappedLights = NULL;
	LIGHT* m_pLights = NULL;
	std::vector<CGameObject*>				m_vGameObjects;
	std::vector<SRV_GAMEOBJECT_INFO>		m_vGameObjectsInfo;
	std::vector<SRV_BILLBOARD_INFO>			m_vBillboardInfo;
	std::vector<SRV_BILLBOARD_INFO>			m_vUploadBillboardInfo;
	std::vector<SRV_UI_INFO>				m_vUIInfo;

	int										m_nShaders = 0;
	int										m_nLights = 0;
	int										m_nVisibleBillboard = 0;
	int                                     m_nCurrentFrameIndex = 0;

	CShader									**m_ppShaders = NULL;
	CPlayer* m_pPlayer = NULL;

	CollisionManager* CM;

	XMFLOAT4								m_xmf4GlobalAmbient;
public:
	UINT									SRVIndex = 0; 

	UINT                                    m_nObjNumCached = 0;
	UINT                                    m_nElemSizeCached = 0;
	UINT                                    m_nTotalSizeCached = 0;

	void AddGameObjectInfo(CGameObject* gameObject, XMFLOAT4X4* parentMatrix = NULL, XMFLOAT4X4* parentModelMatrix = NULL);
	void AddBillboardInfo();
	void UpdateGameObjectINFO(CGameObject* gameObject);
	void UpdateGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateBillboardSRV(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BindGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList, UINT nRootParameterIndex = 1);

public:
	//--[절두체 컬링]--------------------------------------------------------
	bool IsBillboardInFrustum(const SRV_BILLBOARD_INFO& billboard, const XMFLOAT4* planes);
	bool IsInFrustum(const XMFLOAT3& center, float radius, const XMFLOAT4* planes);
	void PerformFrustumCulling(CCamera* pCamera);
	//-----------------------------------------------------------------------
public:
	static DescriptorHeap*				m_pDescriptorHeap;

	static void CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);
	static void CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	static D3D12_GPU_DESCRIPTOR_HANDLE CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride);
	static void CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex);
	static void CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);
	void CreateShaderResourceViews(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()); }
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandleForHeapStart() { return(m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart()); }
};
