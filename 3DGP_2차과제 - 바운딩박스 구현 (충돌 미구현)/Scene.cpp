//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CDescriptorHeap* CScene::m_pDescriptorHeap = NULL;

CDescriptorHeap::CDescriptorHeap()
{
	m_d3dCPUDescriptorHandle.ptr = NULL;
	m_d3dGPUDescriptorHandle.ptr = NULL;
}

CDescriptorHeap::~CDescriptorHeap()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}

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
	// Bounding Box Info SRV
	const UINT numBBXElements = (UINT)m_vBoundingBoxInfo.size();
	const UINT elementBBXSize = sizeof(SRV_BOUNDINGBOX_INFO);
	const UINT totalBBXSize = numBBXElements * elementBBXSize;

	m_pd3dBoundingBoxBuffer = ::CreateBufferResource(
		pd3dDevice, NULL, m_vBoundingBoxInfo.data(), totalBBXSize,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvBBXDesc = {};
	srvBBXDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvBBXDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvBBXDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvBBXDesc.Buffer.FirstElement = 0;
	srvBBXDesc.Buffer.NumElements = numBBXElements;
	srvBBXDesc.Buffer.StructureByteStride = elementBBXSize;
	srvBBXDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(m_pd3dBoundingBoxBuffer, &srvBBXDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

	m_pDescriptorHeap->m_d3dGPUBoundingBoxDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}

void CScene::AddGameObjectInfo(CGameObject* gameObject, XMFLOAT4X4* parentMatrix)
{
	if (!gameObject) return;

	SRV_GAMEOBJECT_INFO info = {};
	XMFLOAT4X4 worldMatrix = (parentMatrix) ? Matrix4x4::Multiply(gameObject->m_xmf4x4Transform, *parentMatrix) : gameObject->m_xmf4x4Transform;
	
	XMMATRIX mtxPWorld = XMLoadFloat4x4(&worldMatrix);

	XMStoreFloat4x4(&info.m_xmf4x4World, XMMatrixTranspose(mtxPWorld));

	if (gameObject->m_nMaterials && gameObject->m_ppMaterials[0]) {
		info.m_material.m_cAmbient = gameObject->m_ppMaterials[0]->m_xmf4AmbientColor;
		info.m_material.m_cDiffuse = gameObject->m_ppMaterials[0]->m_xmf4AlbedoColor;
		info.m_material.m_cEmissive = gameObject->m_ppMaterials[0]->m_xmf4EmissiveColor;
		info.m_material.m_cSpecular = gameObject->m_ppMaterials[0]->m_xmf4SpecularColor;

		info.m_textureMask = gameObject->m_ppMaterials[0]->m_nType;
	}

	m_vGameObjectsInfo.emplace_back(info);

	gameObject->SRVIndex = SRVIndex++;

	AddBoundingBoxInfo(gameObject); 

	if (gameObject->m_pChild) AddGameObjectInfo(gameObject->m_pChild, &worldMatrix);
	if (gameObject->m_pSibling) AddGameObjectInfo(gameObject->m_pSibling, parentMatrix);
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

void CScene::BindGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList, UINT nRootParameterIndex)
{
	const int renderFrame = (m_nCurrentFrameIndex ^ 1);
	pd3dCommandList->SetGraphicsRootDescriptorTable(nRootParameterIndex, m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle[renderFrame]);
}

void CScene::AddBoundingBoxInfo(CGameObject* gameObject)
{
	if (!gameObject->m_nMeshes || !gameObject->m_ppMeshes[0]) return;

	SRV_BOUNDINGBOX_INFO info = {};

	for (int i = 0; i < gameObject->m_nMeshes; ++i) {
		info.m_xmf3AABBCenter = gameObject->m_ppMeshes[i]->GetAABBCenter();
		info.m_xmf3AABBExtents = gameObject->m_ppMeshes[i]->GetAABBExtents();
		info.m_objectIndex = gameObject->SRVIndex;

		m_vBoundingBoxInfo.emplace_back(info);
	}
}