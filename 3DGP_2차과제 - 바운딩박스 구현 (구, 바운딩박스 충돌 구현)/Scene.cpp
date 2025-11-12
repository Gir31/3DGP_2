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

	///////////////////////////////////////////////////////////////////////////////////
	// Bounding Box Info SRV
	const UINT numSphElements = (UINT)m_vSphereInfo.size();
	const UINT elementSphSize = sizeof(SRV_SPHERE_INFO);
	const UINT totalSphSize = numSphElements * elementSphSize;

	m_pd3dSphereBuffer = ::CreateBufferResource(
		pd3dDevice, NULL, m_vSphereInfo.data(), totalSphSize,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvSphDesc = {};
	srvSphDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvSphDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvSphDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvSphDesc.Buffer.FirstElement = 0;
	srvSphDesc.Buffer.NumElements = numSphElements;
	srvSphDesc.Buffer.StructureByteStride = elementSphSize;
	srvSphDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	pd3dDevice->CreateShaderResourceView(m_pd3dSphereBuffer, &srvSphDesc, m_pDescriptorHeap->m_d3dCPUDescriptorHandle);

	m_pDescriptorHeap->m_d3dGPUSphereDescriptorHandle = m_pDescriptorHeap->m_d3dGPUDescriptorHandle;

	m_pDescriptorHeap->m_d3dCPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_pDescriptorHeap->m_d3dGPUDescriptorHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
}

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

	int boxIndexStart = (int)m_vBoundingBoxInfo.size();
	float radius = AddDebugCollisionInfo(gameObject, &modelMatrix);

	if (parentMatrix == NULL)
	{
		SRV_SPHERE_INFO sphereInfo = {};
		sphereInfo.m_fRadius = radius;
		sphereInfo.m_objectIndex = gameObject->SRVIndex;
		sphereInfo.m_collision = false;

		m_vSphereInfo.emplace_back(sphereInfo);
	}
	else
	{
		if (!m_vSphereInfo.empty())
		{
			if (m_vSphereInfo.back().m_fRadius < radius)
				m_vSphereInfo.back().m_fRadius = radius;
		}
	}

	if (gameObject->m_pChild) AddGameObjectInfo(gameObject->m_pChild, &gameObject->m_xmf4x4World, &modelMatrix);
	if (gameObject->m_pSibling) AddGameObjectInfo(gameObject->m_pSibling, parentMatrix, parentModelMatrix);

	if (parentMatrix == NULL)
	{
		int boxIndexEnd = (int)m_vBoundingBoxInfo.size();
		for (int i = boxIndexStart; i < boxIndexEnd; ++i)
			gameObject->m_vBoundingBoxIndices.emplace_back(i);
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

void CScene::BindGameObjectSRV(ID3D12GraphicsCommandList* pd3dCommandList, UINT nRootParameterIndex)
{
	const int renderFrame = (m_nCurrentFrameIndex ^ 1);
	pd3dCommandList->SetGraphicsRootDescriptorTable(nRootParameterIndex, m_pDescriptorHeap->m_d3dGPUObjectDescriptorHandle[renderFrame]);
}

FLOAT CScene::AddDebugCollisionInfo(CGameObject* gameObject, XMFLOAT4X4* modelMatrix)
{
	if (!gameObject || !gameObject->m_nMeshes || !gameObject->m_ppMeshes[0])
		return 0.f;

	SRV_BOUNDINGBOX_INFO info = {};
	float maxRadius = 0.f;

	for (int i = 0; i < gameObject->m_nMeshes; ++i)
	{
		CMesh* pMesh = gameObject->m_ppMeshes[i];
		if (!pMesh) continue; 

		XMFLOAT3 extents = pMesh->GetAABBExtents();

		XMFLOAT3 sphereCenter = pMesh->GetAABBCenter();
		XMFLOAT3 boundingBoxCenter = pMesh->GetAABBCenter();

		if (modelMatrix)
		{
			XMVECTOR vCenter = XMLoadFloat3(&sphereCenter);
			XMMATRIX xmModel = XMLoadFloat4x4(modelMatrix);
			vCenter = XMVector3TransformCoord(vCenter, xmModel);
			XMStoreFloat3(&sphereCenter, vCenter);
		}

		if (extents.x == 0.f && extents.y == 0.f && extents.z == 0.f)
			continue; 

		float cornerX = fabsf(sphereCenter.x) + extents.x;
		float cornerY = fabsf(sphereCenter.y) + extents.y;
		float cornerZ = fabsf(sphereCenter.z) + extents.z;

		float radius = cornerX * cornerX + cornerY * cornerY + cornerZ * cornerZ;

		if (radius > maxRadius)
			maxRadius = radius;

		info.m_xmf3AABBCenter = boundingBoxCenter;
		info.m_xmf3AABBExtents = extents;
		info.m_objectIndex = gameObject->SRVIndex;
		info.m_collision = false;

		m_vBoundingBoxInfo.emplace_back(info);
	}

	return maxRadius;
}

void CScene::CheckSphereCollision(ID3D12GraphicsCommandList* pd3dCommandList)
{
	bool update = false;
	const size_t count = m_vSphereInfo.size();
	if (count < 2) return;

	std::vector<std::pair<UINT, UINT>> collisionSpherePairs;

	for (auto& sphere : m_vSphereInfo)
		sphere.m_collision = false;

	for (auto& boundBox : m_vBoundingBoxInfo)
		boundBox.m_collision = false;

	// 구-구 충돌 검사 (O(n^2))
	for (size_t i = 0; i < count - 1; ++i)
	{
		SRV_SPHERE_INFO& A = m_vSphereInfo[i];
		CGameObject* pObjA = m_vGameObjects[A.m_objectIndex];
		if (!pObjA) continue;

		// 중심 계산 (월드행렬 기반)
		XMFLOAT3 centerA = XMFLOAT3(pObjA->m_xmf4x4World._41, pObjA->m_xmf4x4World._44, pObjA->m_xmf4x4World._43); 

		for (size_t j = i + 1; j < count; ++j)
		{
			SRV_SPHERE_INFO& B = m_vSphereInfo[j];
			CGameObject* pObjB = m_vGameObjects[B.m_objectIndex];
			if (!pObjB) continue;

			XMFLOAT3 centerB = XMFLOAT3(pObjB->m_xmf4x4World._41, pObjB->m_xmf4x4World._44, pObjB->m_xmf4x4World._43);

			// 거리 계산
			float dx = centerA.x - centerB.x;
			float dy = centerA.y - centerB.y;
			float dz = centerA.z - centerB.z;
			float distSq = dx * dx + dy * dy + dz * dz;
			float radiusSum = A.m_fRadius + B.m_fRadius;

			if (distSq <= radiusSum)
			{
				// 충돌 발생
				A.m_collision = true;
				B.m_collision = true;

				collisionSpherePairs.emplace_back(A.m_objectIndex, B.m_objectIndex);

				update = true;
			}
		}
	}

	if (update)
	{
		UpdateSphereSRV(pd3dCommandList);
		CheckOBBCollisionPairs(pd3dCommandList, collisionSpherePairs);
	}
}

void CScene::UpdateSphereSRV(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_pd3dSphereBuffer) return;
	if (m_vSphereInfo.empty()) return;

	const UINT totalSize = (UINT)(sizeof(SRV_SPHERE_INFO) * m_vSphereInfo.size());

	// Sphere 데이터 다시 Map → 복사 → Unmap
	SRV_SPHERE_INFO* pMapped = nullptr;
	HRESULT hr = m_pd3dSphereBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pMapped));
	if (SUCCEEDED(hr))
	{
		memcpy(pMapped, m_vSphereInfo.data(), totalSize);
		m_pd3dSphereBuffer->Unmap(0, nullptr);
	}
}

void CScene::CheckOBBCollisionPairs(ID3D12GraphicsCommandList* pd3dCommandList, const std::vector<std::pair<UINT, UINT>>& pairs)
{
	bool update = false;

	for (const auto& pair : pairs)
	{
		CGameObject* objA = m_vGameObjects[pair.first];
		CGameObject* objB = m_vGameObjects[pair.second];

		if (!objA || !objB) continue;

		for (const auto& boundBoxIndexA : objA->m_vBoundingBoxIndices)
		{
			const SRV_BOUNDINGBOX_INFO& boxA = m_vBoundingBoxInfo[boundBoxIndexA];
			const XMFLOAT4X4& worldA = m_vGameObjects[boxA.m_objectIndex]->m_xmf4x4World;

			for (const auto& boundBoxIndexB : objB->m_vBoundingBoxIndices)
			{
				const SRV_BOUNDINGBOX_INFO& boxB = m_vBoundingBoxInfo[boundBoxIndexB];
				const XMFLOAT4X4& worldB = m_vGameObjects[boxB.m_objectIndex]->m_xmf4x4World;

				// 실제 OBB 충돌 검사
				if (CheckOBBCollision(boxA, worldA, boxB, worldB))
				{
					m_vBoundingBoxInfo[boundBoxIndexA].m_collision = TRUE;
					m_vBoundingBoxInfo[boundBoxIndexB].m_collision = TRUE;

					update = true;
				}
			}
		}
	}

	if (update)
	{
		UpdateBoundindBoxSRV(pd3dCommandList);
	}
}

bool CScene::CheckOBBCollision(const SRV_BOUNDINGBOX_INFO& boxA, const XMFLOAT4X4& worldA,
	const SRV_BOUNDINGBOX_INFO& boxB, const XMFLOAT4X4& worldB)
{
	const float epsilon = 1e-6f;

	// 중심 좌표를 월드좌표계로 변환
	XMFLOAT3 cA = boxA.m_xmf3AABBCenter;
	XMFLOAT3 centerA = Vector3::TransformCoord(cA, const_cast<XMFLOAT4X4&>(worldA));

	XMFLOAT3 cB = boxB.m_xmf3AABBCenter;
	XMFLOAT3 centerB = Vector3::TransformCoord(cB, const_cast<XMFLOAT4X4&>(worldB));

	// A, B의 로컬 축 (Right, Up, Look)
	XMFLOAT3 A_axis[3] =
	{
		XMFLOAT3(worldA._11, worldA._12, worldA._13),
		XMFLOAT3(worldA._21, worldA._22, worldA._23),
		XMFLOAT3(worldA._31, worldA._32, worldA._33)
	};

	XMFLOAT3 B_axis[3] =
	{
		XMFLOAT3(worldB._11, worldB._12, worldB._13),
		XMFLOAT3(worldB._21, worldB._22, worldB._23),
		XMFLOAT3(worldB._31, worldB._32, worldB._33)
	};

	// 각 축 정규화
	for (int i = 0; i < 3; ++i)
	{
		A_axis[i] = Vector3::Normalize(A_axis[i]);
		B_axis[i] = Vector3::Normalize(B_axis[i]);
	}

	// 반경 길이 (Extents)
	XMFLOAT3 eA = boxA.m_xmf3AABBExtents;
	XMFLOAT3 eB = boxB.m_xmf3AABBExtents;

	// 중심 간 벡터 (B - A)
	XMFLOAT3 t = Vector3::Subtract(centerB, centerA);
	// A의 로컬 좌표계로 변환
	t = XMFLOAT3(Vector3::DotProduct(t, A_axis[0]),
		Vector3::DotProduct(t, A_axis[1]),
		Vector3::DotProduct(t, A_axis[2]));

	// 배열 버전으로 변환 (인덱싱용)
	float tArr[3] = { t.x, t.y, t.z };

	float R[3][3], AbsR[3][3];
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			R[i][j] = Vector3::DotProduct(A_axis[i], B_axis[j]);
			AbsR[i][j] = fabsf(R[i][j]) + epsilon;
		}
	}

	float ra, rb;

	// --- 1) A의 축 검사 ---
	for (int i = 0; i < 3; ++i)
	{
		ra = (&eA.x)[i];
		rb = eB.x * AbsR[i][0] + eB.y * AbsR[i][1] + eB.z * AbsR[i][2];
		if (fabsf(tArr[i]) > ra + rb) return false;
	}

	// --- 2) B의 축 검사 ---
	for (int i = 0; i < 3; ++i)
	{
		ra = eA.x * AbsR[0][i] + eA.y * AbsR[1][i] + eA.z * AbsR[2][i];
		rb = (&eB.x)[i];
		float proj = fabsf(t.x * R[0][i] + t.y * R[1][i] + t.z * R[2][i]);
		if (proj > ra + rb) return false;
	}

	// --- 3) A×B 축 검사 ---
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			ra = eA.y * AbsR[(i + 2) % 3][j] + eA.z * AbsR[(i + 1) % 3][j];
			rb = eB.y * AbsR[i][(j + 2) % 3] + eB.z * AbsR[i][(j + 1) % 3];

			float proj = fabsf(tArr[(i + 2) % 3] * R[(i + 1) % 3][j] -
				tArr[(i + 1) % 3] * R[(i + 2) % 3][j]);

			if (proj > ra + rb) return false;
		}
	}

	return true; // 분리축이 없음 → 충돌 발생
}

void CScene::UpdateBoundindBoxSRV(ID3D12GraphicsCommandList* pd3dCommandList)
{
	if (!m_pd3dBoundingBoxBuffer) return;
	if (m_vSphereInfo.empty()) return;

	const UINT totalSize = (UINT)(sizeof(SRV_BOUNDINGBOX_INFO) * m_vBoundingBoxInfo.size());

	// Sphere 데이터 다시 Map → 복사 → Unmap
	SRV_BOUNDINGBOX_INFO* pMapped = nullptr;
	HRESULT hr = m_pd3dBoundingBoxBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pMapped));
	if (SUCCEEDED(hr))
	{
		memcpy(pMapped, m_vBoundingBoxInfo.data(), totalSize);
		m_pd3dBoundingBoxBuffer->Unmap(0, nullptr);
	}
}