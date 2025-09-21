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

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	BuildDefaultLightsAndMaterials();

	m_nGameObjects = 7;
	m_ppGameObjects = new CGameObject*[m_nGameObjects];

	CGameObject *pApacheModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Apache.bin");
	CApacheObject* pApacheObject = NULL;

	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel, true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(+130.0f, 0.0f, 160.0f);
	pApacheObject->SetScale(1.5f, 1.5f, 1.5f);
	pApacheObject->Rotate(0.0f, 90.0f, 0.0f);
	m_ppGameObjects[0] = pApacheObject;

	pApacheObject = new CApacheObject();
	pApacheObject->SetChild(pApacheModel, true);
	pApacheObject->OnInitialize();
	pApacheObject->SetPosition(-75.0f, 0.0f, 80.0f);
	pApacheObject->SetScale(1.5f, 1.5f, 1.5f);
	pApacheObject->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[1] = pApacheObject;

	CGameObject *pGunshipModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Gunship.bin");
	CGunshipObject* pGunshipObject = NULL;

	pGunshipObject = new CGunshipObject();
	pGunshipObject->SetChild(pGunshipModel, true);
	pGunshipObject->OnInitialize();
	pGunshipObject->SetPosition(135.0f, 40.0f, 220.0f);
	pGunshipObject->SetScale(8.5f, 8.5f, 8.5f);
	pGunshipObject->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[2] = pGunshipObject;

	CGameObject *pSuperCobraModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/SuperCobra.bin");
	CSuperCobraObject* pSuperCobraObject = NULL;

	pSuperCobraObject = new CSuperCobraObject();
	pSuperCobraObject->SetChild(pSuperCobraModel, true);
	pSuperCobraObject->OnInitialize();
	pSuperCobraObject->SetPosition(95.0f, 50.0f, 50.0f);
	pSuperCobraObject->SetScale(4.5f, 4.5f, 4.5f);
	pSuperCobraObject->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[3] = pSuperCobraObject;

	CGameObject *pMi24Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Mi24.bin");
	CMi24Object* pMi24Object = new CMi24Object();
	pMi24Object->SetChild(pMi24Model, true);
	pMi24Object->OnInitialize();
	pMi24Object->SetPosition(-95.0f, 50.0f, 50.0f);
	pMi24Object->SetScale(4.5f, 4.5f, 4.5f);
	pMi24Object->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[4] = pMi24Object;

	CGameObject* pHummerModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Hummer.bin");
	CHummerObject* pHummerObject = new CHummerObject();
	pHummerObject->SetChild(pHummerModel);
	pHummerObject->OnInitialize();
	pHummerObject->SetPosition(260.0f, 0.0f, 150.0f);
	pHummerObject->SetScale(18.0f, 18.0f, 18.0f);
	pHummerObject->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[5] = pHummerObject;

	CGameObject* pAbramsModel = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/M26.bin");
	CM26Object* pTankObject = new CM26Object();
	pTankObject->SetChild(pAbramsModel);
	pTankObject->OnInitialize();
	pTankObject->SetPosition(260.0f, 0.0f, 150.0f);
	pTankObject->SetScale(18.0f, 18.0f, 18.0f);
	pTankObject->Rotate(0.0f, -90.0f, 0.0f);
	m_ppGameObjects[6] = pTankObject;

	// --------------------------
	// 5. 디스크립터 힙 생성
	// --------------------------
	UINT nCbvs = m_nGameObjects + 1; // 오브젝트 CBV들 + Light CBV
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = nCbvs;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	pd3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_pd3dCbvSrvDescriptorHeap));

	UINT nDescSize = pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 시작 핸들
	m_d3dObjectsCbvCPUDescriptorHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dObjectsCbvGPUDescriptorHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	// 라이트는 오브젝트들 뒤에 위치
	m_d3dLightsCbvCPUDescriptorHandle = m_d3dObjectsCbvCPUDescriptorHandle;
	m_d3dLightsCbvCPUDescriptorHandle.ptr += (SIZE_T)(nDescSize * m_nGameObjects);

	m_d3dLightsCbvGPUDescriptorHandle = m_d3dObjectsCbvGPUDescriptorHandle;
	m_d3dLightsCbvGPUDescriptorHandle.ptr += (UINT64)(nDescSize * m_nGameObjects);

	// --------------------------
// 6. 오브젝트 CBV 생성
// --------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE objCpuHandle = m_d3dObjectsCbvCPUDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE objGpuHandle = m_d3dObjectsCbvGPUDescriptorHandle;

	for (int i = 0; i < m_nGameObjects; ++i)
	{
		CGameObject* pObj = m_ppGameObjects[i];

		// 1) CBV용 리소스 생성
		UINT nBytes = (sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255; // 256바이트 정렬
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC resDesc = {};
		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resDesc.Width = nBytes;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.SampleDesc.Count = 1;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		pd3dDevice->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
			IID_PPV_ARGS(&pObj->m_pd3dcbGameObject));

		// 2) 맵핑
		pObj->m_pd3dcbGameObject->Map(0, nullptr, (void**)&pObj->m_pcbMappedGameObject);

		// 3) 초기 데이터 작성
		XMFLOAT4X4 worldT;
		XMStoreFloat4x4(&worldT, XMMatrixTranspose(XMLoadFloat4x4(&pObj->m_xmf4x4World)));
		pObj->m_pcbMappedGameObject->m_xmf4x4World = worldT;

		if (pObj->m_nMaterials > 0 && pObj->m_ppMaterials[0] && pObj->m_ppMaterials[0]->m_pMaterialColors)
		{
			auto* c = pObj->m_ppMaterials[0]->m_pMaterialColors;
			pObj->m_pcbMappedGameObject->m_Material.m_cAmbient = c->m_xmf4Ambient;
			pObj->m_pcbMappedGameObject->m_Material.m_cDiffuse = c->m_xmf4Diffuse;
			pObj->m_pcbMappedGameObject->m_Material.m_cSpecular = c->m_xmf4Specular;
			pObj->m_pcbMappedGameObject->m_Material.m_cEmissive = c->m_xmf4Emissive;
		}
		else
		{
			pObj->m_pcbMappedGameObject->m_Material.m_cAmbient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
			pObj->m_pcbMappedGameObject->m_Material.m_cDiffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			pObj->m_pcbMappedGameObject->m_Material.m_cSpecular = XMFLOAT4(0.5f, 0.5f, 0.5f, 8.0f);
			pObj->m_pcbMappedGameObject->m_Material.m_cEmissive = XMFLOAT4(0, 0, 0, 0);
		}

		// 4) CBV 디스크립터 생성
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = pObj->m_pd3dcbGameObject->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = nBytes;

		pd3dDevice->CreateConstantBufferView(&cbvDesc, objCpuHandle);

		// 5) GPU 핸들 저장 (핸들을 통째로 저장)
		pObj->SetCbvGPUDescriptorHandle(objGpuHandle.ptr);

		// 6) 핸들 이동 (★ 오브젝트마다 한 칸씩!)
		objCpuHandle.ptr += nDescSize;
		objGpuHandle.ptr += nDescSize;
	}



	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UINT nLightBytes = (sizeof(LIGHTS) + 255) & ~255;
	D3D12_CONSTANT_BUFFER_VIEW_DESC lightCbvDesc = {};
	lightCbvDesc.BufferLocation = m_pd3dcbLights->GetGPUVirtualAddress();
	lightCbvDesc.SizeInBytes = nLightBytes;

	pd3dDevice->CreateConstantBufferView(&lightCbvDesc, m_d3dLightsCbvCPUDescriptorHandle);
	
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	// -------------------------------
	// Descriptor Ranges 정의
	// -------------------------------
	D3D12_DESCRIPTOR_RANGE d3dDescriptorRanges[2];

	// GameObject용 (register(b2))
	d3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRanges[0].NumDescriptors = 1;
	d3dDescriptorRanges[0].BaseShaderRegister = 2;
	d3dDescriptorRanges[0].RegisterSpace = 0;
	d3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = 0;

	// Lights용 (register(b4))
	d3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	d3dDescriptorRanges[1].NumDescriptors = 1;
	d3dDescriptorRanges[1].BaseShaderRegister = 4;
	d3dDescriptorRanges[1].RegisterSpace = 0;
	d3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = 0;

	// -------------------------------
	// Root Parameters 정의
	// -------------------------------
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];

	// Camera (b1) → CBV 그대로
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1;
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// GameObject (b2) → Descriptor Table
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &d3dDescriptorRanges[0];
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Lights (b4) → Descriptor Table
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[2].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[2].DescriptorTable.pDescriptorRanges = &d3dDescriptorRanges[1];
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// -------------------------------
	// Root Signature Flags
	// -------------------------------
	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	// -------------------------------
	// Root Signature 생성
	// -------------------------------
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
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

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
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

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	// 1) Descriptor Heap 세팅
	ID3D12DescriptorHeap* ppHeaps[] = { m_pd3dCbvSrvDescriptorHeap };
	pd3dCommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dLightsCbvGPUDescriptorHandle);

	for (int i = 0; i < m_nGameObjects; ++i)
	{
		CGameObject* pObj = m_ppGameObjects[i];

		// 월드행렬 갱신 → 맵에 써주기
		XMFLOAT4X4 worldT;
		XMStoreFloat4x4(&worldT, XMMatrixTranspose(XMLoadFloat4x4(&pObj->m_xmf4x4World)));
		pObj->m_pcbMappedGameObject->m_xmf4x4World = worldT;

		// 머티리얼 값도 필요시 갱신
		if (pObj->m_nMaterials > 0 && pObj->m_ppMaterials[0] && pObj->m_ppMaterials[0]->m_pMaterialColors) {
			auto* c = pObj->m_ppMaterials[0]->m_pMaterialColors;
			pObj->m_pcbMappedGameObject->m_Material.m_cAmbient = c->m_xmf4Ambient;
			pObj->m_pcbMappedGameObject->m_Material.m_cDiffuse = c->m_xmf4Diffuse;
			pObj->m_pcbMappedGameObject->m_Material.m_cSpecular = c->m_xmf4Specular;
			pObj->m_pcbMappedGameObject->m_Material.m_cEmissive = c->m_xmf4Emissive;
		}

		// 오브젝트 Descriptor Table 바인딩 (slot 1)
		pd3dCommandList->SetGraphicsRootDescriptorTable(1, pObj->GetCbvGPUDescriptorHandle());

		// 실제 메쉬 렌더링
		if (pObj->m_pMesh) pObj->m_pMesh->Render(pd3dCommandList);
	}
}

