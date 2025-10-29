#include "stdafx.h"
#include "Scene.h"
#include "Level.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TitleLevel
TitleLevel::TitleLevel()
{
}
TitleLevel::~TitleLevel()
{
}

bool TitleLevel::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool TitleLevel::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void TitleLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void TitleLevel::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void TitleLevel::ReleaseShaderVariables()
{
}

void TitleLevel::BuildDefaultLightsAndMaterials()
{
}

void TitleLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void TitleLevel::ReleaseObjects()
{
}	

void TitleLevel::AnimateObjects(float fTimeElapsed)
{
}

void TitleLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void TitleLevel::ReleaseUploadBuffers()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MenuLevel
MenuLevel::MenuLevel()
{
}
MenuLevel::~MenuLevel()
{
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

void MenuLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void MenuLevel::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void MenuLevel::ReleaseShaderVariables()
{
}

void MenuLevel::BuildDefaultLightsAndMaterials()
{
}

void MenuLevel::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void MenuLevel::ReleaseObjects()
{
}

void MenuLevel::AnimateObjects(float fTimeElapsed)
{
}

void MenuLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
}

void MenuLevel::ReleaseUploadBuffers()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MainLevel
MainLevel::MainLevel()
{
}
MainLevel::~MainLevel()
{
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

void MainLevel::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	UINT bytes = (UINT)(sizeof(VS_VB_INSTANCE) * inst_nGameObjects);
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, Instance_data.data(),
		bytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
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
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
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
	char debugMsg[128];
	sprintf_s(debugMsg, "Load MainLevel....\n");
	OutputDebugStringA(debugMsg); // Visual Studio의 Output 창에 출력됨

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.NumDescriptors = 2; // 충분히 크게, 오브젝트 개수 이상
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	pd3dDevice->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&m_pd3dCbvSrvDescriptorHeap));

	UINT nIncrementSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_CPU_DESCRIPTOR_HANDLE d3dCPUHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGPUHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	m_nGameObjects = 0;
	m_ppGameObjects = new CGameObject * [m_nGameObjects];

	// INSTANCE
	int xObjects = 10, yObjects = 10, zObjects = 10;
	inst_nGameObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	inst_ppGameObjects = new CM26Object();

	float fxPitch = 200.0f * 2.5f;
	float fyPitch = 200.0f * 2.5f;
	float fzPitch = 200.0f * 2.5f;

	CGameObject* inst_pAbramsModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/M26.bin");

	Instance_data.reserve(inst_nGameObjects);

	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				XMMATRIX Scaling = XMMatrixScaling(18.f, 18.f, 18.f);
				XMMATRIX Rotation = XMMatrixRotationY(90.f);
				XMMATRIX Translation = XMMatrixTranslation(fxPitch * x, fyPitch * y, fzPitch * z);
				//XMMATRIX Translation = XMMatrixTranslation(0,0,0);
				XMMATRIX WorldMatrix = Scaling * Rotation * Translation;

				VS_VB_INSTANCE data{};
				XMStoreFloat4x4(&data.m_xmf4x4World, XMMatrixTranspose(WorldMatrix));
				//data.material = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				Instance_data.push_back(data);
			}
		}
	}

	inst_ppGameObjects->SetChild(inst_pAbramsModel, true);
	inst_ppGameObjects->OnInitialize();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	pAirplanePlayer->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pPlayer = pAirplanePlayer;

	for (int i = 0; i < m_nGameObjects; i++) {
		AssignCbvRecursive(m_ppGameObjects[i], pd3dDevice, pd3dCommandList,
			d3dCPUHandle, d3dGPUHandle, nIncrementSize);
	}


	AssignCbvRecursive(inst_ppGameObjects, pd3dDevice, pd3dCommandList,
		d3dCPUHandle, d3dGPUHandle, nIncrementSize);

	AssignCbvRecursive(pAirplanePlayer, pd3dDevice, pd3dCommandList,
		d3dCPUHandle, d3dGPUHandle, nIncrementSize);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// === [SRV 생성 추가] ===
	d3dCPUHandle.ptr += nIncrementSize;
	d3dGPUHandle.ptr += nIncrementSize;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = inst_nGameObjects;
	srvDesc.Buffer.StructureByteStride = sizeof(VS_VB_INSTANCE);
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	// 현재 d3dCPUHandle / d3dGPUHandle은 CBV들을 전부 생성하고 “다음 빈 슬롯” 상태
	pd3dDevice->CreateShaderResourceView(m_pd3dcbGameObjects, &srvDesc, d3dCPUHandle);

	// SRV의 GPU 핸들 저장
	m_d3dInstancingSrvGpuHandle = d3dGPUHandle;

	// 다음 슬롯 이동 (혹시 이후 다른 SRV 만들 경우 대비)
	d3dCPUHandle.ptr += nIncrementSize;
	d3dGPUHandle.ptr += nIncrementSize;
}

void MainLevel::ReleaseObjects()
{
	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (inst_ppGameObjects)
	{
		if (inst_ppGameObjects) inst_ppGameObjects->Release();
	}

	Instance_data.clear();
	Instance_data.shrink_to_fit();

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

void MainLevel::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void MainLevel::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	// 1) 루트 시그니처 설정
	pd3dCommandList->SetGraphicsRootSignature(pd3dGraphicsRootSignature);

	// 2) 디스크립터 힙 바인딩 (Object CBV용)
	ID3D12DescriptorHeap* ppHeaps[] = { m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 3) 카메라 업데이트
	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	// 4) Lights 업데이트
	UpdateShaderVariables(pd3dCommandList);
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress);

	// 추가: SRV 테이블 설정 (RootParam #3)
	pd3dCommandList->SetGraphicsRootDescriptorTable(3, m_d3dInstancingSrvGpuHandle);

	// 5) 오브젝트 렌더링
	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObjects[i])
		{
			m_ppGameObjects[i]->Animate(m_fElapsedTime, NULL);
			m_ppGameObjects[i]->UpdateTransform(NULL);
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}


	if (inst_ppGameObjects)
	{
		inst_ppGameObjects->Animate(m_fElapsedTime, NULL);

		inst_ppGameObjects->m_xmf4x4Transform = Matrix4x4::Identity();
		inst_ppGameObjects->UpdateTransform(NULL);

		inst_ppGameObjects->Render(pd3dCommandList, pCamera, inst_nGameObjects);
	}
}

void MainLevel::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

void MainLevel::Reset()
{

}