#include "stdafx.h"
#include "Scene.h"
#include "Level.h"

//-----------------------------------------------------------------------------
// MenuLevel
//-----------------------------------------------------------------------------
MenuLevel::MenuLevel()
{
}

MenuLevel::~MenuLevel()
{
}

void MenuLevel::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void MenuLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CObjectsShader* pObjectsShader = new CObjectsShader();
	int nObjects = pObjectsShader->GetNumberOfObjects();

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();
	CBoundingBoxShader* pBoundingBoxShader = new CBoundingBoxShader(); 
	CSphereShader* pSphereShader = new CSphereShader();

	m_pDescriptorHeap = new CDescriptorHeap();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 + 3 + 1); //SuperCobra(17), Gunship(2), Player(1), Skybox(1), Terrain(3)

	BuildDefaultLightsAndMaterials();

	m_nShaders = 3;
	m_ppShaders = new CShader * [m_nShaders];

	m_ppShaders[0] = pSkyBoxShader;
	m_ppShaders[1] = pObjectsShader;
	m_ppShaders[2] = pTerrainShader;

	m_nDebugShaders = 2;
	m_ppDebugShaders = new CShader * [m_nDebugShaders];

	m_ppDebugShaders[0] = pBoundingBoxShader;
	m_ppDebugShaders[1] = pSphereShader;


	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	for (int i = 0; i < m_nDebugShaders; i++)
	{
		m_ppDebugShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppDebugShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(920.0f, 745.0f, 1270.0));

	m_pPlayer = pAirplanePlayer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void MenuLevel::ReleaseObjects()
{
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	ReleaseShaderVariables();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_ppDebugShaders)
	{
		for (int i = 0; i < m_nDebugShaders; i++)
		{
			m_ppDebugShaders[i]->ReleaseShaderVariables();
			m_ppDebugShaders[i]->ReleaseObjects();
			m_ppDebugShaders[i]->Release();
		}
		delete[] m_ppDebugShaders;
	}

	if (m_pLights) delete[] m_pLights;
}

void MenuLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	m_vGameObjectsInfo.clear();
	m_vBoundingBoxInfo.clear();
	SRVIndex = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		for (int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(m_ppShaders[i]->m_ppGameObject[j]);
	}

	for (int i = 0; i < m_nDebugShaders; i++)
	{
		for (int j = 0; j < m_ppDebugShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(m_ppDebugShaders[i]->m_ppGameObject[j]);
	}

	AddGameObjectInfo(m_pPlayer);

	((CBoundingBoxShader*)m_ppDebugShaders[0])->m_nBoxes = (UINT)m_vBoundingBoxInfo.size();
	((CSphereShader*)m_ppDebugShaders[1])->m_nSphere = (UINT)m_vSphereInfo.size();

	CreateShaderResourceView(pd3dDevice, pd3dCommandList);
}

void MenuLevel::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void MenuLevel::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	m_vGameObjectsInfo.clear();
	m_vGameObjectsInfo.shrink_to_fit();
	SRVIndex = 0;
}

void MenuLevel::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nDebugShaders; i++) m_ppDebugShaders[i]->ReleaseUploadBuffers();
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

void MenuLevel::AnimateObjects(float fTimeElapsed, CCamera* pCamera)
{
	if (pCamera)
	{
		XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
		m_ppShaders[0]->m_ppGameObject[0]->SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);
	}

	for (int i = 0; i < m_nShaders; i++) {
		if (m_ppShaders[i]) {
			m_ppShaders[i]->AnimateObjects(fTimeElapsed);

			for (int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
				UpdateGameObjectINFO(m_ppShaders[i]->m_ppGameObject[j]);
		}
	}

	UpdateGameObjectINFO(m_pPlayer);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void MenuLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ID3D12DescriptorHeap* heaps[] = { m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	UpdateGameObjectSRV(pd3dCommandList);
	BindGameObjectSRV(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootDescriptorTable(14, m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle); //BoundingBox
	pd3dCommandList->SetGraphicsRootDescriptorTable(15, m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle); //Sphere

	for (int i = 0; i < m_nShaders; i++) {
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}

	if (m_ppDebugShaders[0] && debugCollisionBoundingBox)
		m_ppDebugShaders[0]->Render(pd3dCommandList, pCamera);
	if (m_ppDebugShaders[1] && debugCollisionSphere)
		m_ppDebugShaders[1]->Render(pd3dCommandList, pCamera);
}

//-----------------------------------------------------------------------------
// MainLevel
//-----------------------------------------------------------------------------
MainLevel::MainLevel()
{
}

MainLevel::~MainLevel()
{
}

void MainLevel::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void MainLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CObjectsShader* pObjectsShader = new CObjectsShader();
	int nObjects = pObjectsShader->GetNumberOfObjects();

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();
	CBillboardShader* pBillboardShader = new CBillboardShader();
	CPlayerShader* pPlayerShader = new CPlayerShader();

	m_pDescriptorHeap = new CDescriptorHeap();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 + 3 + 1); //SuperCobra(17), Gunship(2), Player(1), Skybox(1), Terrain(3)

	BuildDefaultLightsAndMaterials();

	m_nShaders = 4;
	m_ppShaders = new CShader * [m_nShaders];

	m_ppShaders[0] = pSkyBoxShader;
	m_ppShaders[1] = pObjectsShader;
	m_ppShaders[2] = pTerrainShader;
	m_ppShaders[3] = pBillboardShader;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(920.0f, 745.0f, 1270.0));

	m_pPlayer = pAirplanePlayer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void MainLevel::ReleaseObjects()
{
	if (m_pDescriptorHeap) delete m_pDescriptorHeap;

	ReleaseShaderVariables();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pLights) delete[] m_pLights;
}

void MainLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	m_vGameObjectsInfo.clear();
	SRVIndex = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		for(int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(m_ppShaders[i]->m_ppGameObject[j]);
	}

	AddGameObjectInfo(m_pPlayer);

	CreateShaderResourceView(pd3dDevice, pd3dCommandList);
}

void MainLevel::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void MainLevel::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	m_vGameObjectsInfo.clear();
	m_vGameObjectsInfo.shrink_to_fit();
	SRVIndex = 0;
}

void MainLevel::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
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

void MainLevel::AnimateObjects(float fTimeElapsed, CCamera* pCamera)
{
	if (pCamera)
	{ 
		XMFLOAT3 xmf3CameraPos = pCamera->GetPosition(); 
		m_ppShaders[0]->m_ppGameObject[0]->SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z); 
	}

	for (int i = 0; i < m_nShaders; i++) {
		if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

		for (int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
			UpdateGameObjectINFO(m_ppShaders[i]->m_ppGameObject[j]);
	}

	UpdateGameObjectINFO(m_pPlayer);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void MainLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	ID3D12DescriptorHeap* heaps[] = { m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	UpdateGameObjectSRV(pd3dCommandList);
	BindGameObjectSRV(pd3dCommandList);


	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);
}