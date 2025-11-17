#include "stdafx.h"
#include "Scene.h"
#include "Level.h"

//-----------------------------------------------------------------------------
// MenuLevel
//-----------------------------------------------------------------------------
void MenuLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CEnemyShader* pObjectsShader = new CEnemyShader();
	int nObjects = pObjectsShader->GetNumberOfObjects();

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();

	m_pDescriptorHeap = new DescriptorHeap();
	CM = new CollisionManager();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 + 3 + 1 +5); //SuperCobra(17), Gunship(2), Player(1), Skybox(1), Terrain(3)

	BuildDefaultLightsAndMaterials();

	m_nShaders = 3;
	m_ppShaders = new CShader * [m_nShaders];

	m_ppShaders[SKY_BOX_SHADER] = pSkyBoxShader;
	m_ppShaders[TERRAIN_SHADER] = pTerrainShader;
	m_ppShaders[ENEMY_SHADER] = pObjectsShader;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	CM->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(920.0f, 745.0f, 1270.0));
	pAirplanePlayer->m_CollisionGroup = CG_PLAYER;

	m_pPlayer = pAirplanePlayer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool MenuLevel::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool MenuLevel::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}
//-----------------------------------------------------------------------------
// MainLevel
//-----------------------------------------------------------------------------
void MainLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CEnemyShader* pObjectsShader = new CEnemyShader();
	int nObjects = pObjectsShader->GetNumberOfObjects();

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();
	CBillboardShader* pBillboardShader = new CBillboardShader();
	CUIShader* pUIShader = new CUIShader();

	m_pDescriptorHeap = new DescriptorHeap();
	CM = new CollisionManager();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 + 3 + 1); //SuperCobra(17), Gunship(2), Player(1), Skybox(1), Terrain(3)

	BuildDefaultLightsAndMaterials();

	m_nShaders = 5;
	m_ppShaders = new CShader * [m_nShaders];

	m_ppShaders[SKY_BOX_SHADER] = pSkyBoxShader;
	m_ppShaders[TERRAIN_SHADER] = pTerrainShader;
	m_ppShaders[ENEMY_SHADER] = pObjectsShader;
	m_ppShaders[BILLBOARD_SHADER] = pBillboardShader;
	m_ppShaders[UI_SHADER] = pUIShader;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	CM->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(920.0f, 745.0f, 1270.0));
	pAirplanePlayer->m_CollisionGroup = CG_PLAYER;

	m_pPlayer = pAirplanePlayer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool MainLevel::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool MainLevel::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}