#include "stdafx.h"
#include "Scene.h"
#include "Level.h"

//-----------------------------------------------------------------------------
// MenuLevel
//-----------------------------------------------------------------------------
void MenuLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	m_vGameObjectsInfo.clear();
	SRVIndex = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		for (int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(m_ppShaders[i]->m_ppGameObject[j]);
	}

	for (int i = 0; i < CM->m_nDebugShaders; i++)
	{
		for (int j = 0; j < CM->m_ppDebugShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(CM->m_ppDebugShaders[i]->m_ppGameObject[j]);
	}

	AddGameObjectInfo(m_pPlayer);


	//==[UI]======================================
	SRV_UI_INFO ui = {};

	ui.uiTexturesMask = 2;
	ui.xmf2Size = XMFLOAT2(0.5f, 0.2f);
	ui.xmf3Location = XMFLOAT3(0.f, 0.f, 0.f);

	m_vUIInfo.emplace_back(ui);

	SRV_UI_INFO ui2 = {};

	ui2.uiTexturesMask = 3;
	ui2.xmf2Size = XMFLOAT2(0.4f, 0.2f);
	ui2.xmf3Location = XMFLOAT3(0.f, -0.4f, 0.f);

	m_vUIInfo.emplace_back(ui2);


	((CUIShader*)m_pUIShader)->m_nUI = (UINT)m_vUIInfo.size();
	//============================================

	CM->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList);
}

void MenuLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();
	CUIShader* pUIShader = new CUIShader();

	m_pDescriptorHeap = new DescriptorHeap();
	CM = new CollisionManager();
	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 17 + 50 + 1 + 1 + 3 + 1 +5);

	BuildDefaultLightsAndMaterials();

	m_nShaders = 2;
	m_ppShaders = new CShader * [m_nShaders];

	m_ppShaders[SKY_BOX_SHADER] = pSkyBoxShader;
	m_ppShaders[TERRAIN_SHADER] = pTerrainShader;

	m_pUIShader = pUIShader;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	m_pUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);

	CM->BuildObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(920.0f, 745.0f, 1270.0));
	pAirplanePlayer->m_CollisionGroup = CG_PLAYER;

	m_pPlayer = pAirplanePlayer;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

bool MenuLevel::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		int mouseX = (int)(short)LOWORD(lParam);
		int mouseY = (int)(short)HIWORD(lParam);



		if (pickingUI(m_vUIInfo[0].xmf3Location, m_vUIInfo[0].xmf2Size, mouseX, mouseY)) {
			return (true);
		}
		else if (pickingUI(m_vUIInfo[1].xmf3Location, m_vUIInfo[1].xmf2Size, mouseX, mouseY))
			::PostQuitMessage(0);
	}
	break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}

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

void MenuLevel::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature,
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	ID3D12DescriptorHeap* heaps[] = { m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	UpdateSRV(
		pd3dDevice, pd3dCommandList,
		m_vGameObjectsInfo.data(), m_vGameObjectsInfo.size(),
		sizeof(SRV_GAMEOBJECT_INFO), m_pObjectDefault, m_pObjectUpload);

	pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle);

	PerformFrustumCulling(pCamera);
	CM->CheckSphereCollision(pd3dCommandList, m_vGameObjects);

	pd3dCommandList->SetGraphicsRootDescriptorTable(14, m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle); //BoundingBox
	pd3dCommandList->SetGraphicsRootDescriptorTable(15, m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle); //Sphere
	pd3dCommandList->SetGraphicsRootDescriptorTable(17, m_pDescriptorHeap->m_d3dGPUUIDescriptorHandle); // UI

	for (int i = 0; i < m_nShaders; i++) {
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera, 0);
	}

	//if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);

	if (m_pUIShader)
		m_pUIShader->Render(pd3dCommandList, pCamera, 0);


	CM->Render(pd3dCommandList, pCamera);
}

bool MenuLevel::pickingUI(const XMFLOAT3& xmf3Center, const XMFLOAT2& xmf2Size, const int& mouseX, const int& mouseY)
{
	float halfWidth = xmf2Size.x * 0.5f;
	float halfHeight = xmf2Size.y * 0.5f;

	float x = ((float)mouseX / g_nScreenWidth) * 2.f - 1.f;
	float y = ((float)mouseY / g_nScreenHeight) * -2.f + 1.f;

	float left = xmf3Center.x - halfWidth;
	float right = xmf3Center.x + halfWidth;
	float top = xmf3Center.y - halfHeight;
	float bottom = xmf3Center.y + halfHeight;

	// 콘솔 출력
	printf("Mouse LClick: X=%.3f, Y=%.3f\n", x, y);

	// Visual Studio Output 창에 출력
	char buffer[128];
	sprintf_s(buffer, "Mouse LClick: X=%.3f, Y=%.3f\n", x, y);
	OutputDebugStringA(buffer);

	if (x > left && x < right &&
		y > top && y < bottom)
	{
		return true;
	}


	return false;
}

//-----------------------------------------------------------------------------
// MainLevel
//-----------------------------------------------------------------------------
void MainLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	AddBillboardInfo();

	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	m_vGameObjectsInfo.clear();
	SRVIndex = 0;

	for (int i = 0; i < m_nShaders; i++)
	{
		for (int j = 0; j < m_ppShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(m_ppShaders[i]->m_ppGameObject[j]);
	}

	for (int i = 0; i < CM->m_nDebugShaders; i++)
	{
		for (int j = 0; j < CM->m_ppDebugShaders[i]->m_nGameObjects; j++)
			AddGameObjectInfo(CM->m_ppDebugShaders[i]->m_ppGameObject[j]);
	}

	AddGameObjectInfo(m_pPlayer);


	//==[UI]======================================
	SRV_UI_INFO ui = {};

	ui.uiTexturesMask = 0;
	ui.xmf2Size = XMFLOAT2(0.8f, 0.8f);
	ui.xmf3Location = XMFLOAT3(0.f, -0.9f, 0.f);

	m_vUIInfo.emplace_back(ui);

	SRV_UI_INFO ui2 = {};

	ui2.uiTexturesMask = 1;
	ui2.xmf2Size = XMFLOAT2(0.8f, 0.8f);
	ui2.xmf3Location = XMFLOAT3(0.f, -0.9f, 0.f);

	m_vUIInfo.emplace_back(ui2);

	SRV_UI_INFO ui3 = {};

	ui3.uiTexturesMask = 4;
	ui3.xmf2Size = XMFLOAT2(0.1f, 1.f);
	ui3.xmf3Location = XMFLOAT3(-0.75f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui3);

	SRV_UI_INFO ui4 = {};

	ui4.uiTexturesMask = 5;
	ui4.xmf2Size = XMFLOAT2(0.1f, 0.1f);
	ui4.xmf3Location = XMFLOAT3(-0.85f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui4);

	SRV_UI_INFO ui5 = {};

	ui5.uiTexturesMask = 6;
	ui5.xmf2Size = XMFLOAT2(0.1f, 0.1f);
	ui5.xmf3Location = XMFLOAT3(-0.88f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui5);

	SRV_UI_INFO ui6 = {};

	ui6.uiTexturesMask = 7;
	ui6.xmf2Size = XMFLOAT2(0.1f, 0.1f);
	ui6.xmf3Location = XMFLOAT3(-0.91f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui6);

	SRV_UI_INFO ui7 = {};

	ui7.uiTexturesMask = 8;
	ui7.xmf2Size = XMFLOAT2(0.1f, 0.1f);
	ui7.xmf3Location = XMFLOAT3(-0.94f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui7);

	SRV_UI_INFO ui8 = {};

	ui8.uiTexturesMask = 9;
	ui8.xmf2Size = XMFLOAT2(0.1f, 1.f);
	ui8.xmf3Location = XMFLOAT3(0.75f, 0.0f, 0.f);

	m_vUIInfo.emplace_back(ui8);

	((CUIShader*)m_pUIShader)->m_nUI = (UINT)m_vUIInfo.size();
	//============================================

	//==[MIRROR]==================================
	SRV_RECT_INFO mirror1 = {};

	mirror1.xmf2Size = XMFLOAT2(1000.f, 1000.f);
	mirror1.xmf3Center = XMFLOAT3(920.0f, 1000.f, 1270.0);

	m_vMirror.emplace_back(mirror1);
	//============================================

	if (m_nShaders > 3)
		((CBillboardShader*)m_ppShaders[3])->m_nBillboard = (UINT)m_vBillboardInfo.size();

	CM->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateShaderResourceViews(pd3dDevice, pd3dCommandList);
}

void MainLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CEnemyShader* pObjectsShader = new CEnemyShader();
	int nObjects = pObjectsShader->GetNumberOfObjects();

	CSkyBoxShader* pSkyBoxShader = new CSkyBoxShader();
	CTerrainShader* pTerrainShader = new CTerrainShader();
	CBillboardShader* pBillboardShader = new CBillboardShader();
	CUIShader* pUIShader = new CUIShader();
	CMirrorShader* pMirrorShader = new CMirrorShader(); 

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
	m_ppShaders[MIRROR_SHADER] = pMirrorShader;

	m_pUIShader = pUIShader;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}

	m_pUIShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_pUIShader->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);

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

void MainLevel::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature,
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	ID3D12DescriptorHeap* heaps[] = { m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(heaps), heaps);
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	UpdateSRV(
		pd3dDevice, pd3dCommandList,
		m_vGameObjectsInfo.data(), m_vGameObjectsInfo.size(),
		sizeof(SRV_GAMEOBJECT_INFO), m_pObjectDefault, m_pObjectUpload);

	pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle);

	PerformFrustumCulling(pCamera);
	CM->CheckSphereCollision(pd3dCommandList, m_vGameObjects);

	if (!m_vUploadBillboardInfo.empty())
		UpdateSRV(
			pd3dDevice, pd3dCommandList,
			m_vUploadBillboardInfo.data(), m_vUploadBillboardInfo.size(),
			sizeof(SRV_RECT_INFO), m_pBillboardsDefault, m_pBillboardsUpload);

	if (m_nShaders > 3)
		((CBillboardShader*)m_ppShaders[3])->m_nBillboard = (UINT)m_nVisibleBillboard;

	pd3dCommandList->SetGraphicsRootDescriptorTable(14, m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle); //BoundingBox
	pd3dCommandList->SetGraphicsRootDescriptorTable(15, m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle); //Sphere
	pd3dCommandList->SetGraphicsRootDescriptorTable(16, m_pDescriptorHeap->m_d3dGPUBillboardDescriptorHandle); //Sphere
	pd3dCommandList->SetGraphicsRootDescriptorTable(17, m_pDescriptorHeap->m_d3dGPUUIDescriptorHandle); // UI
	pd3dCommandList->SetGraphicsRootDescriptorTable(21, m_pDescriptorHeap->m_d3dGPUMirrorDescriptorHandle); // 



	for (int i = 0; i < m_nShaders; i++) {
		if (i == MIRROR_SHADER) continue;

		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera, 0);
	}

	pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
	pd3dCommandList->OMSetStencilRef(1);
	if (m_ppShaders[MIRROR_SHADER])
		m_ppShaders[MIRROR_SHADER]->Render(pd3dCommandList, pCamera, 0);

	pd3dCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

	CCamera* reflectionCamera = new CCamera(pCamera);
	reflectionCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	ComputeMirrorReflectionCamera(pCamera, reflectionCamera);
	reflectionCamera->GenerateViewMatrix(
		reflectionCamera->GetPosition(),
		reflectionCamera->GetLookAtPosition(),
		reflectionCamera->GetUpVector()
	);
	reflectionCamera->SetViewportsAndScissorRects(pd3dCommandList);
	reflectionCamera->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->OMSetStencilRef(1);
	for (int i = 0; i < m_nShaders; i++) {
		if (i == MIRROR_SHADER) continue;

		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, reflectionCamera, 1);
	}
	//if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, reflectionCamera);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, pCamera);

	if (m_pUIShader)
		m_pUIShader->Render(pd3dCommandList, pCamera, 0);


	CM->Render(pd3dCommandList, pCamera);
}