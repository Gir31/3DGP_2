//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
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

void CScene::AssignCbvRecursive(CGameObject* pObject,
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	D3D12_CPU_DESCRIPTOR_HANDLE& d3dCPUHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE& d3dGPUHandle,
	UINT nIncrementSize)
{
	if (!pObject) return;

	// 자기 자신의 CBV 할당
	pObject->SetCbvDescriptorHandles(d3dCPUHandle, d3dGPUHandle);
	pObject->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	// 다음 슬롯으로 이동
	d3dCPUHandle.ptr += nIncrementSize;
	d3dGPUHandle.ptr += nIncrementSize;

	// 자식과 형제도 재귀적으로 처리
	if (pObject->m_pChild)   AssignCbvRecursive(pObject->m_pChild, pd3dDevice, pd3dCommandList,
		d3dCPUHandle, d3dGPUHandle, nIncrementSize);
	if (pObject->m_pSibling) AssignCbvRecursive(pObject->m_pSibling, pd3dDevice, pd3dCommandList,
		d3dCPUHandle, d3dGPUHandle, nIncrementSize);
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

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
	int xObjects = 20, yObjects = 20, zObjects = 20, inst = 0;
	inst_nGameObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	inst_ppGameObjects = new CGameObject();

	float fxPitch = 200.0f * 2.5f;
	float fyPitch = 200.0f * 2.5f;
	float fzPitch = 200.0f * 2.5f;

	CGameObject* inst_pAbramsModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/M26.bin");

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

	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(pd3dDevice, pd3dCommandList, GetGraphicsRootSignature());
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

	UpdateInstanceShaderVariables(pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (inst_ppGameObjects)
	{
		if (inst_ppGameObjects) inst_ppGameObjects->Release();
		delete[] inst_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE descRange = {};
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRange.NumDescriptors = 1;
	descRange.BaseShaderRegister = 2; // register(b2)
	descRange.RegisterSpace = 0;
	descRange.OffsetInDescriptorsFromTableStart = 0;

	D3D12_DESCRIPTOR_RANGE srvRange = {};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;
	srvRange.BaseShaderRegister = 0; // t0
	srvRange.RegisterSpace = 0;
	srvRange.OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &descRange;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &srvRange;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[4].Constants.Num32BitValues = 1;
	pd3dRootParameters[4].Constants.ShaderRegister = 3; // b3
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);

	UINT bytes = (UINT)(((sizeof(VS_VB_INSTANCE) + 255) & ~255) * inst_nGameObjects);
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, Instance_data.data(),
		bytes,D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::UpdateInstanceShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{

}

void CScene::FillInstanceRecursive(CGameObject* pObject, int& index)
{
	if (!pObject || index >= inst_nGameObjects) return;

	pObject->UpdateTransform(NULL);

	XMStoreFloat4x4(
		&m_pcbMappedGameObjects[index].m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(&pObject->m_xmf4x4World))
	);

	if (pObject->m_pChild)
		FillInstanceRecursive(pObject->m_pChild, index);

	if (pObject->m_pSibling)
		FillInstanceRecursive(pObject->m_pSibling, index);
}


void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'W': m_ppGameObjects[0]->MoveForward(+1.0f); break;
		case 'S': m_ppGameObjects[0]->MoveForward(-1.0f); break;
		case 'A': m_ppGameObjects[0]->MoveStrafe(-1.0f); break;
		case 'D': m_ppGameObjects[0]->MoveStrafe(+1.0f); break;
		case 'Q': m_ppGameObjects[0]->MoveUp(+1.0f); break;
		case 'R': m_ppGameObjects[0]->MoveUp(-1.0f); break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	m_fElapsedTime = fTimeElapsed;

	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// 1) 루트 시그니처 설정
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

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