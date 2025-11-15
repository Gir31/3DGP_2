//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [CScene]
void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
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

	if(m_nShaders > 3)
		((CBillboardShader*)m_ppShaders[3])->m_nBillboard = (UINT)m_vBillboardInfo.size();

	CM->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	CreateShaderResourceView(pd3dDevice, pd3dCommandList);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
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

void CScene::AnimateObjects(float fTimeElapsed, CCamera* pCamera)
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

void CScene::Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature)
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

	PerformFrustumCulling(pCamera);
	CM->CheckSphereCollision(pd3dCommandList, m_vGameObjects);
	UpdateBillboardSRV(pd3dDevice, pd3dCommandList);

	if (m_nShaders > 3)
		((CBillboardShader*)m_ppShaders[3])->m_nBillboard = (UINT)m_nVisibleBillboard;

	pd3dCommandList->SetGraphicsRootDescriptorTable(14, m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle); //BoundingBox
	pd3dCommandList->SetGraphicsRootDescriptorTable(15, m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle); //Sphere
	pd3dCommandList->SetGraphicsRootDescriptorTable(16, m_pDescriptorHeap->m_d3dGPUBillboardDescriptorHandle); //Sphere

	for (int i = 0; i < m_nShaders; i++) {
		if (m_ppShaders[i])
			m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}

	CM->Render(pd3dCommandList, pCamera);
}

void CScene::ReleaseObjects()
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
	CM->ReleaseObject();
	

	if (CM) delete CM;
	if (m_pLights) delete[] m_pLights;
}
void CScene::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	CM->ReleaseUploadBuffers();
}
void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	for (int i = 0; i < 2; ++i)
	{
		if (m_pd3dGameObjects[i])
		{
			m_pd3dGameObjects[i]->Unmap(0, NULL);
			m_pd3dGameObjects[i]->Release();
		}
		if (m_pd3dUploadBuffer[i])
		{
			m_pd3dUploadBuffer[i]->Unmap(0, NULL);
			m_pd3dUploadBuffer[i]->Release();
		}

	}

	m_vGameObjectsInfo.clear();
	m_vGameObjectsInfo.shrink_to_fit();
	SRVIndex = 0;

	CM->ReleaseShaderVariables();
}

//=====[CBV SRV 생성]===============================================
DescriptorHeap* CScene::m_pDescriptorHeap = NULL;

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap);

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle = m_pDescriptorHeap->m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
}

void CScene::CreateConstantBufferViews(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = pd3dConstantBuffer->GetGPUVirtualAddress();
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;
	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferView(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress;
	pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;
	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	return(d3dCbvGPUDescriptorHandle);
}

void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	int nTextures = pTexture->GetTextures();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource* pShaderResource = pTexture->GetResource(i);
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(i, m_pDescriptorHeap->m_d3dGPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int i = 0; i < nRootParameters; i++) pTexture->SetRootParameterIndex(i, nRootParameterStartIndex + i);
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dGPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetRootParameterIndex(nIndex, nRootParameterStartIndex + nIndex);
	}
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex)
{
	ID3D12Resource* pShaderResource = pTexture->GetResource(nIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE d3dGpuDescriptorHandle = pTexture->GetGpuDescriptorHandle(nIndex);
	if (pShaderResource && !d3dGpuDescriptorHandle.ptr)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(nIndex);
		pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

		pTexture->SetGpuDescriptorHandle(nIndex, m_pDescriptorHeap->m_d3dGPUDescriptorHandle);
		m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}
}

void CScene::CreateShaderResourceView(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	const UINT numElements = (UINT)m_vGameObjectsInfo.size();
	const UINT elementSize = sizeof(SRV_GAMEOBJECT_INFO);
	const UINT totalSize = numElements * elementSize;

	m_nObjNumCached = numElements;
	m_nElemSizeCached = elementSize;
	m_nTotalSizeCached = totalSize;

	for (int i = 0; i < 2; ++i)
	{
		if (m_pd3dGameObjects[i]) { m_pd3dGameObjects[i]->Release(); m_pd3dGameObjects[i] = nullptr; }
		if (m_pd3dUploadBuffer[i]) { m_pd3dUploadBuffer[i]->Release();  m_pd3dUploadBuffer[i] = nullptr; }

		m_pd3dGameObjects[i] = ::CreateBufferResource(
			pd3dDevice, nullptr, nullptr, totalSize,
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

		m_pd3dUploadBuffer[i] = ::CreateBufferResource(
			pd3dDevice, nullptr, m_vGameObjectsInfo.data(), totalSize,
			D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

		pd3dCommandList->CopyResource(m_pd3dGameObjects[i], m_pd3dUploadBuffer[i]);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_pd3dGameObjects[i];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		pd3dCommandList->ResourceBarrier(1, &barrier);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = numElements;
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	for (int i = 0; i < 2; ++i)
	{
		pd3dDevice->CreateShaderResourceView(m_pd3dGameObjects[i], &srvDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

		m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle[i] = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

		m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	}

	m_nCurrentFrameIndex = 0;

	///////////////////////////////////////////////////////////////////////////////////
	// Billboard Info SRV
	const UINT numBBElements = (UINT)m_vBillboardInfo.size();
	const UINT elementBBSize = sizeof(SRV_BILLBOARD_INFO);
	const UINT totalBBSize = numBBElements * elementBBSize;

	m_pd3dBillboards = ::CreateBufferResource(
		pd3dDevice, NULL, m_vBillboardInfo.data(), totalBBSize,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvBBDesc = {};
	srvBBDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvBBDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvBBDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvBBDesc.Buffer.FirstElement = 0;
	srvBBDesc.Buffer.NumElements = numBBElements;
	srvBBDesc.Buffer.StructureByteStride = elementBBSize;
	srvBBDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(m_pd3dBillboards, &srvBBDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

	m_pDescriptorHeap->m_d3dGPUBillboardDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	///////////////////////////////////////////////////////////////////////////////////
	// Bounding Box Info SRV
	const UINT numBBXElements = (UINT)CM->m_vBoundingBoxInfo.size();
	const UINT elementBBXSize = sizeof(SRV_BOUNDINGBOX_INFO);
	const UINT totalBBXSize = numBBXElements * elementBBXSize;

	CM->m_pd3dBoundingBoxBuffer = ::CreateBufferResource(
		pd3dDevice, NULL, CM->m_vBoundingBoxInfo.data(), totalBBXSize,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvBBXDesc = {};
	srvBBXDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvBBXDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvBBXDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvBBXDesc.Buffer.FirstElement = 0;
	srvBBXDesc.Buffer.NumElements = numBBXElements;
	srvBBXDesc.Buffer.StructureByteStride = elementBBXSize;
	srvBBXDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(CM->m_pd3dBoundingBoxBuffer, &srvBBXDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

	m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	///////////////////////////////////////////////////////////////////////////////////
	// Sphere Info SRV
	const UINT numSphElements = (UINT)CM->m_vSphereInfo.size();
	const UINT elementSphSize = sizeof(SRV_SPHERE_INFO);
	const UINT totalSphSize = numSphElements * elementSphSize;

	CM->m_pd3dSphereBuffer = ::CreateBufferResource(
		pd3dDevice, NULL, CM->m_vSphereInfo.data(), totalSphSize,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvSphDesc = {};
	srvSphDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvSphDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvSphDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvSphDesc.Buffer.FirstElement = 0;
	srvSphDesc.Buffer.NumElements = numSphElements;
	srvSphDesc.Buffer.StructureByteStride = elementSphSize;
	srvSphDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(CM->m_pd3dSphereBuffer, &srvSphDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

	m_pDescriptorHeap->m_d3dCPUBillboardDescriptorHandle = m_pDescriptorHeap->m_d3dCPUDescriptorHandle;
	m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}
//==================================================================

//=====[힙 업데이트]================================================
void CScene::AddGameObjectInfo(CGameObject* gameObject, XMFLOAT4X4* parentMatrix, XMFLOAT4X4* parentModelMatrix)
{
	if (!gameObject) return;

	SRV_GAMEOBJECT_INFO info = {};
	gameObject->m_xmf4x4World = (parentMatrix) ? Matrix4x4::Multiply(gameObject->m_xmf4x4Transform, *parentMatrix) : gameObject->m_xmf4x4Transform;
	XMFLOAT4X4 modelMatrix = (parentModelMatrix) ? Matrix4x4::Multiply(gameObject->m_xmf4x4Transform, *parentModelMatrix) : Matrix4x4::Identity();
	
	XMMATRIX mtxPWorld = XMLoadFloat4x4(&gameObject->m_xmf4x4World);

	XMStoreFloat4x4(&info.m_xmf4x4World, XMMatrixTranspose(mtxPWorld));

	if (gameObject->m_nMaterials && gameObject->m_ppMaterials[0]) {
		info.m_material.m_cAmbient = gameObject->m_ppMaterials[0]->m_xmf4AmbientColor;
		info.m_material.m_cDiffuse = gameObject->m_ppMaterials[0]->m_xmf4AlbedoColor;
		info.m_material.m_cEmissive = gameObject->m_ppMaterials[0]->m_xmf4EmissiveColor;
		info.m_material.m_cSpecular = gameObject->m_ppMaterials[0]->m_xmf4SpecularColor;

		info.m_textureMask = gameObject->m_ppMaterials[0]->m_nType;
	}

	m_vGameObjectsInfo.emplace_back(info);
	m_vGameObjects.emplace_back(gameObject);

	gameObject->SRVIndex = SRVIndex++;

	int boxIndexStart = (int)CM->m_vBoundingBoxInfo.size();
	bool Root = (parentModelMatrix) ? false : true;
	CM->AddCollisionInfo(*gameObject, modelMatrix, Root);

	if (gameObject->m_pChild) AddGameObjectInfo(gameObject->m_pChild, &gameObject->m_xmf4x4World, &modelMatrix);
	if (gameObject->m_pSibling) AddGameObjectInfo(gameObject->m_pSibling, parentMatrix, parentModelMatrix);

	if (Root)
	{
		int boxIndexEnd = (int)CM->m_vBoundingBoxInfo.size();
		for (int i = boxIndexStart; i < boxIndexEnd; ++i)
			gameObject->m_vBoundingBoxIndices.emplace_back(i);
	}
}

void CScene::AddBillboardInfo()
{
	if (m_nShaders > BILLBOARD_SHADER)
	{
		CHeightMapTerrain* pTerrain = (CHeightMapTerrain*)m_ppShaders[TERRAIN_SHADER]->m_ppGameObject[0];

		int nTerrainWidth = int(pTerrain->GetWidth());
		int nTerrainLength = int(pTerrain->GetLength());
		XMFLOAT3 xmf3Scale = pTerrain->GetScale();

		m_vBillboardInfo.reserve(nTerrainWidth * nTerrainLength);

		for (int x = 0; x < nTerrainWidth; ++x)
		{
			for (int z = 0; z < nTerrainLength; ++z)
			{
				SRV_BILLBOARD_INFO billboard = {};

				billboard.xmf3Center.x = x * xmf3Scale.x;
				billboard.xmf3Center.z = z * xmf3Scale.z;
				billboard.xmf3Center.y = pTerrain->GetHeight(billboard.xmf3Center.x, billboard.xmf3Center.z);


				billboard.xmf2Size = XMFLOAT2(100.f, 100.f);

				m_vBillboardInfo.emplace_back(billboard);
			}
		}
	}
}

void CScene::UpdateGameObjectINFO(CGameObject* gameObject)
{
	XMStoreFloat4x4(&(m_vGameObjectsInfo[gameObject->SRVIndex].m_xmf4x4World), XMMatrixTranspose(XMLoadFloat4x4(&(gameObject->m_xmf4x4World))));

	if (gameObject->m_pChild) UpdateGameObjectINFO(gameObject->m_pChild);
	if (gameObject->m_pSibling) UpdateGameObjectINFO(gameObject->m_pSibling);
}

void CScene::UpdateGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_pd3dGameObjects[0] || !m_pd3dGameObjects[1]) return;
	if (m_vGameObjectsInfo.empty()) return;

	const UINT64 totalSize = sizeof(SRV_GAMEOBJECT_INFO) * m_vGameObjectsInfo.size();

	const int frame = m_nCurrentFrameIndex;     
	ID3D12Resource* pDstDefault = m_pd3dGameObjects[frame];
	ID3D12Resource* pSrcUpload = m_pd3dUploadBuffer[frame];

	SRV_GAMEOBJECT_INFO* pMapped = nullptr;
	pSrcUpload->Map(0, nullptr, reinterpret_cast<void**>(&pMapped));
	memcpy(pMapped, m_vGameObjectsInfo.data(), totalSize);
	pSrcUpload->Unmap(0, nullptr);

	{
		D3D12_RESOURCE_BARRIER toCopy = {};
		toCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toCopy.Transition.pResource = pDstDefault;
		toCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		toCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
		toCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		pd3dCommandList->ResourceBarrier(1, &toCopy);

		pd3dCommandList->CopyResource(pDstDefault, pSrcUpload);

		D3D12_RESOURCE_BARRIER toRead = {};
		toRead.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		toRead.Transition.pResource = pDstDefault;
		toRead.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		toRead.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		toRead.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		pd3dCommandList->ResourceBarrier(1, &toRead);
	}

	m_nCurrentFrameIndex = (m_nCurrentFrameIndex + 1) & 1;
}

void CScene::UpdateBillboardSRV(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (m_vUploadBillboardInfo.empty()) return;

	const UINT numElements = (UINT)m_nVisibleBillboard;
	const UINT elementSize = sizeof(SRV_BILLBOARD_INFO);
	const UINT totalSize = numElements * elementSize;

	SRV_BILLBOARD_INFO* pMapped = nullptr;
	m_pd3dBillboards->Map(0, nullptr, reinterpret_cast<void**>(&pMapped));
	memcpy(pMapped, m_vUploadBillboardInfo.data(), totalSize);
	m_pd3dBillboards->Unmap(0, nullptr);

	// 2) SRV NumElements 업데이트 (가장 중요)
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = numElements;         // ★ 업데이트되는 부분
	srvDesc.Buffer.StructureByteStride = elementSize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(
		m_pd3dBillboards,
		&srvDesc,
		m_pDescriptorHeap->m_d3dCPUBillboardDescriptorHandle // 같은 slot에 덮어쓰기
	);
}
//==================================================================

void CScene::BindGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList, UINT nRootParameterIndex)
{
	const int renderFrame = (m_nCurrentFrameIndex ^ 1);
	pd3dCommandList->SetGraphicsRootDescriptorTable(nRootParameterIndex, m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle[renderFrame]);
}

//=====[절두체 컬링]================================================
bool CScene::IsBillboardInFrustum(const SRV_BILLBOARD_INFO& billboard, const XMFLOAT4* planes)
{
	const XMFLOAT3& center = billboard.xmf3Center;

	float radius = max(billboard.xmf2Size.x, billboard.xmf2Size.y) * 0.5f;

#if define FULL_INCLUSION_TEST
	for (int i = 0; i < 6; ++i)
	{
		const XMFLOAT4& p = planes[i];
		float distance = p.x * center.x + p.y * center.y + p.z * center.z + p.w;

		if (distance < -radius)
			return false; 
	}
#else
	for (int i = 0; i < 6; ++i)
	{
		const XMFLOAT4& p = planes[i];
		float distance = p.x * center.x + p.y * center.y + p.z * center.z + p.w;

		if (distance <= radius)
			return false;
	}
#endif

	return true;
}

bool CScene::IsInFrustum(const XMFLOAT3& center, float radius, const XMFLOAT4* planes)
{
#if define FULL_INCLUSION_TEST
	// 부분 내부 판정
	for (int i = 0; i < 6; ++i)
	{
		float distance = planes[i].x * center.x + planes[i].y * center.y + planes[i].z * center.z + planes[i].w;
		if (distance < -radius)
			return false;
	}
#else
	// 완전 내부 판정
	for (int i = 0; i < 6; ++i)
	{
		float distance = planes[i].x * center.x + planes[i].y * center.y + planes[i].z * center.z + planes[i].w;
		if (distance <= radius)  // 반지름까지 포함해서 내부에 안 들어가면 제외
			return false;
	}
#endif

	return true;
}

void CScene::PerformFrustumCulling(CCamera* pCamera)
{
	XMMATRIX view = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&pCamera->GetProjectionMatrix());
	XMMATRIX xmmViewProj = XMMatrixMultiply(view, proj);

	XMFLOAT4 planes[6];
	// Left
	XMStoreFloat4(&planes[0], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[3] + xmmViewProj.r[0].m128_f32[0],
		xmmViewProj.r[1].m128_f32[3] + xmmViewProj.r[1].m128_f32[0],
		xmmViewProj.r[2].m128_f32[3] + xmmViewProj.r[2].m128_f32[0],
		xmmViewProj.r[3].m128_f32[3] + xmmViewProj.r[3].m128_f32[0])));
	// Right
	XMStoreFloat4(&planes[1], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[3] - xmmViewProj.r[0].m128_f32[0],
		xmmViewProj.r[1].m128_f32[3] - xmmViewProj.r[1].m128_f32[0],
		xmmViewProj.r[2].m128_f32[3] - xmmViewProj.r[2].m128_f32[0],
		xmmViewProj.r[3].m128_f32[3] - xmmViewProj.r[3].m128_f32[0])));
	// Top
	XMStoreFloat4(&planes[2], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[3] - xmmViewProj.r[0].m128_f32[1],
		xmmViewProj.r[1].m128_f32[3] - xmmViewProj.r[1].m128_f32[1],
		xmmViewProj.r[2].m128_f32[3] - xmmViewProj.r[2].m128_f32[1],
		xmmViewProj.r[3].m128_f32[3] - xmmViewProj.r[3].m128_f32[1])));
	// Bottom
	XMStoreFloat4(&planes[3], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[3] + xmmViewProj.r[0].m128_f32[1],
		xmmViewProj.r[1].m128_f32[3] + xmmViewProj.r[1].m128_f32[1],
		xmmViewProj.r[2].m128_f32[3] + xmmViewProj.r[2].m128_f32[1],
		xmmViewProj.r[3].m128_f32[3] + xmmViewProj.r[3].m128_f32[1])));
	// Near
	XMStoreFloat4(&planes[4], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[2],
		xmmViewProj.r[1].m128_f32[2],
		xmmViewProj.r[2].m128_f32[2],
		xmmViewProj.r[3].m128_f32[2])));
	// Far
	XMStoreFloat4(&planes[5], XMPlaneNormalize(XMVectorSet(
		xmmViewProj.r[0].m128_f32[3] - xmmViewProj.r[0].m128_f32[2],
		xmmViewProj.r[1].m128_f32[3] - xmmViewProj.r[1].m128_f32[2],
		xmmViewProj.r[2].m128_f32[3] - xmmViewProj.r[2].m128_f32[2],
		xmmViewProj.r[3].m128_f32[3] - xmmViewProj.r[3].m128_f32[2])));


	for (auto& sphere : CM->m_vSphereInfo) // ← 당신의 SRV_SPHERE_INFO 벡터
	{
		if (sphere.m_fRadius <= 0.f) continue;

		CGameObject* obj = m_vGameObjects[sphere.m_objectIndex];
		XMFLOAT3 objPos = obj->GetPosition();
		obj->m_bVisible = IsInFrustum(objPos, sphere.m_fRadius, planes);
	}

	m_vUploadBillboardInfo.clear();
	m_nVisibleBillboard = 0;

	for (auto& billboard : m_vBillboardInfo)
	{
		if (IsBillboardInFrustum(billboard, planes))
		{
			m_vUploadBillboardInfo.emplace_back(billboard);
			m_nVisibleBillboard++;
		}
	}
}
//==================================================================