#include "stdafx.h"
#include "Object.h"
#include "Shader.h"
#include "CollisionManager.h"

CollisionManager::CollisionManager()
{
	m_pd3dSphereBuffer = NULL;
	m_pd3dBoundingBoxBuffer = NULL;

	m_vSphereInfo.clear();
	m_vBoundingBoxInfo.clear();

	DebugSphereRender = false;
	DebugBoundingBoxRender = false;

	SST = new SPHERE_SPHERE_TEST();
	GJK = new GILBERT_JOHNSON_KEERTHI();
	SAT = new SEPARATING_AXIS_THEOREM();
}

CollisionManager::~CollisionManager()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [디버깅 렌더링]
void CollisionManager::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	((CSphereShader*)m_ppDebugShaders[BOUNDING_SPHERE_SHADER])->m_nSphere = (UINT)m_vSphereInfo.size();
	((CBoundingBoxShader*)m_ppDebugShaders[BOUNDING_BOX_SHADER])->m_nBoxes = (UINT)m_vBoundingBoxInfo.size();
}

void CollisionManager::BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	CSphereShader* pSphereShader = new CSphereShader();
	CBoundingBoxShader* pBoundingBoxShader = new CBoundingBoxShader();

	m_nDebugShaders = 2;
	m_ppDebugShaders = new CShader * [m_nDebugShaders];

	m_ppDebugShaders[BOUNDING_SPHERE_SHADER] = pSphereShader;
	m_ppDebugShaders[BOUNDING_BOX_SHADER] = pBoundingBoxShader;

	for (int i = 0; i < m_nDebugShaders; i++)
	{
		m_ppDebugShaders[i]->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
		m_ppDebugShaders[i]->BuildObjects(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, NULL);
	}
}

void CollisionManager::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_ppDebugShaders[BOUNDING_SPHERE_SHADER] && DebugBoundingBoxRender)
		m_ppDebugShaders[BOUNDING_SPHERE_SHADER]->Render(pd3dCommandList, pCamera);
	if (m_ppDebugShaders[BOUNDING_BOX_SHADER] && DebugSphereRender)
		m_ppDebugShaders[BOUNDING_BOX_SHADER]->Render(pd3dCommandList, pCamera);
}

void CollisionManager::ReleaseObject()
{
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
}

void CollisionManager::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nDebugShaders; i++) m_ppDebugShaders[i]->ReleaseUploadBuffers();
}

void CollisionManager::ReleaseShaderVariables()
{
	if (m_pd3dSphereBuffer)
	{
		m_pd3dSphereBuffer->Unmap(0, NULL);
		m_pd3dSphereBuffer->Release();
	}

	if (m_pd3dBoundingBoxBuffer)
	{
		m_pd3dBoundingBoxBuffer->Unmap(0, NULL);
		m_pd3dBoundingBoxBuffer->Release();
	}

	m_vSphereInfo.clear();
	m_vSphereInfo.shrink_to_fit();

	m_vBoundingBoxInfo.clear();
	m_vBoundingBoxInfo.shrink_to_fit();

	if (SST) delete SST;
	if (GJK) delete GJK;
	if (SAT) delete SAT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [충돌 정보 추가]
void CollisionManager::AddCollisionInfo(CGameObject& gameObject, XMFLOAT4X4& xmf4x4ModelMatrix, bool Root)
{
	float MaxRadiusSquared = 0.f;

	for (int i = 0; i < gameObject.m_nMeshes; ++i)
	{
		if (!gameObject.m_ppMeshes[i]) continue;
		CMesh* pMesh = gameObject.m_ppMeshes[i];

		AddBoundingBox(pMesh->GetAABBCenter(), pMesh->GetAABBExtents(), gameObject.SRVIndex);

		float RadiusSquared = CalculateRadiusSquared(pMesh->GetAABBCenter(), pMesh->GetAABBExtents(), xmf4x4ModelMatrix);

		if (RadiusSquared > MaxRadiusSquared) MaxRadiusSquared = RadiusSquared;
	}

	AddSphere(gameObject, MaxRadiusSquared, Root);
}

void CollisionManager::AddSphere(CGameObject& gameObject, float Radius, bool Root)
{
	if (Root)
	{
		SRV_SPHERE_INFO sphereInfo = {};
		sphereInfo.m_fRadius = sqrtf(Radius);
		sphereInfo.m_objectIndex = gameObject.SRVIndex;
		sphereInfo.m_collision = false;

		m_vSphereInfo.emplace_back(sphereInfo);
	}
	else
	{
		if (!m_vSphereInfo.empty())
		{
			if ((m_vSphereInfo.back().m_fRadius * m_vSphereInfo.back().m_fRadius) < Radius)
				m_vSphereInfo.back().m_fRadius = sqrtf(Radius);
		}
	}
}

void CollisionManager::AddBoundingBox(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const UINT Idx)
{
	SRV_BOUNDINGBOX_INFO info = {};

	if (xmf3MeshExtents.x == 0.f && xmf3MeshExtents.y == 0.f && xmf3MeshExtents.z == 0.f)
		return;

	info.m_xmf3AABBCenter = xmf3MeshCenter;
	info.m_xmf3AABBExtents = xmf3MeshExtents;
	info.m_objectIndex = Idx;
	info.m_collision = false;

	m_vBoundingBoxInfo.emplace_back(info);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [충돌 정보 업데이트]

void CollisionManager::UpdateCollisionSRV(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateSphereSRV(pd3dCommandList);
	UpdateBoundindBoxSRV(pd3dCommandList);
}

void CollisionManager::UpdateSphereSRV(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CollisionManager::UpdateBoundindBoxSRV(ID3D12GraphicsCommandList* pd3dCommandList)
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [기타 정보]
FLOAT CollisionManager::CalculateRadiusSquared(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const XMFLOAT4X4& xmf4x4Model)
{
	XMVECTOR vCenter = XMLoadFloat3(&xmf3MeshCenter);
	XMMATRIX xmModel = XMLoadFloat4x4(&xmf4x4Model);

	vCenter = XMVector3TransformCoord(vCenter, xmModel);

	XMFLOAT3 transformedCenter;
	XMStoreFloat3(&transformedCenter, vCenter);

	float cornerX = fabsf(transformedCenter.x) + xmf3MeshExtents.x;
	float cornerY = fabsf(transformedCenter.y) + xmf3MeshExtents.y;
	float cornerZ = fabsf(transformedCenter.z) + xmf3MeshExtents.z;

	return (cornerX * cornerX + cornerY * cornerY + cornerZ * cornerZ);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [충돌 체크]
void CollisionManager::CheckSphereCollision(ID3D12GraphicsCommandList* pd3dCommandList, std::vector<CGameObject*>& gameObjects)
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
		CGameObject* pObjA = gameObjects[A.m_objectIndex];
		if (!pObjA) continue;
		if (pObjA->m_CollisionGroup & NONE_CHECK) continue;

		for (size_t j = i + 1; j < count; ++j)
		{
			SRV_SPHERE_INFO& B = m_vSphereInfo[j];
			CGameObject* pObjB = gameObjects[B.m_objectIndex];
			if (!pObjB) continue;
			if (pObjB->m_CollisionGroup & NONE_CHECK) continue;
			if ((pObjA->m_CollisionGroup | pObjB->m_CollisionGroup) & ALL_STATIC) continue;
			
			int check = pObjA->m_CollisionGroup | pObjB->m_CollisionGroup;

			int CollisionGroup = pObjA->m_CollisionGroup | pObjB->m_CollisionGroup;

			if (SST->SphereIntersection(A.m_fRadius, pObjA->m_xmf4x4World, B.m_fRadius, pObjB->m_xmf4x4World))
			{
				// 충돌 발생
				A.m_collision = true;
				B.m_collision = true;

				collisionSpherePairs.emplace_back(A.m_objectIndex, B.m_objectIndex);

				update = true;
			}
		}
	}

	UpdateSphereSRV(pd3dCommandList);

	if (update)
	{
		CheckOBBCollisionPairs(pd3dCommandList, gameObjects, collisionSpherePairs);
	}
}

void CollisionManager::CheckOBBCollisionPairs
(ID3D12GraphicsCommandList* pd3dCommandList, 
	std::vector<CGameObject*>& gameObjects,
	const std::vector<std::pair<UINT, UINT>>& pairs)
{
	bool update = false;

	for (const auto& pair : pairs)
	{
		CGameObject* objA = gameObjects[pair.first];
		CGameObject* objB = gameObjects[pair.second];

		const UINT& collisionGroupA = objA->m_CollisionGroup;
		const UINT& collisionGroupB = objB->m_CollisionGroup;

		if (!objA || !objB) continue;

		for (const auto& boundBoxIndexA : objA->m_vBoundingBoxIndices)
		{
			const SRV_BOUNDINGBOX_INFO& boxA = m_vBoundingBoxInfo[boundBoxIndexA];
			const XMFLOAT4X4& worldA = gameObjects[boxA.m_objectIndex]->m_xmf4x4World;
			

			for (const auto& boundBoxIndexB : objB->m_vBoundingBoxIndices)
			{
				const SRV_BOUNDINGBOX_INFO& boxB = m_vBoundingBoxInfo[boundBoxIndexB];
				const XMFLOAT4X4& worldB = gameObjects[boxB.m_objectIndex]->m_xmf4x4World;
				

				// 실제 OBB 충돌 검사
				if (CheckCollisionGroup(boxA, worldA, collisionGroupA, boxB, worldB, collisionGroupB))
				{
					m_vBoundingBoxInfo[boundBoxIndexA].m_collision = TRUE;
					m_vBoundingBoxInfo[boundBoxIndexB].m_collision = TRUE;

					update = true;
				}
			}
		}
	}

	UpdateBoundindBoxSRV(pd3dCommandList);
}

bool CollisionManager::CheckCollisionGroup(const SRV_BOUNDINGBOX_INFO& boxA, const XMFLOAT4X4& worldA, const UINT& collisionGroupA,
	const SRV_BOUNDINGBOX_INFO& boxB, const XMFLOAT4X4& worldB, const UINT& collisionGroupB)
{
	UINT CollisionGroup = collisionGroupA | collisionGroupB;

	switch (CollisionGroup)
	{
	case PLAYER_TO_ENEMY:
		return (GJK->OBBIntersection(boxA.m_xmf3AABBCenter, boxA.m_xmf3AABBExtents, worldA,
			boxB.m_xmf3AABBCenter, boxB.m_xmf3AABBExtents, worldB));
	case PLAYER_TO_ITEM:
		return false;
	case PLAYER_TO_PROJECTILE:
		return (GJK->OBBIntersection(boxA.m_xmf3AABBCenter, boxA.m_xmf3AABBExtents, worldA,
			boxB.m_xmf3AABBCenter, boxB.m_xmf3AABBExtents, worldB));
	case ENEMY_TO_ENEMY:
		/*return (SAT->OBBIntersection(boxA.m_xmf3AABBCenter, boxA.m_xmf3AABBExtents, worldA,
			boxB.m_xmf3AABBCenter, boxB.m_xmf3AABBExtents, worldB));*/
		return false;
	case ENEMY_TO_PROJECTILE:
		return (GJK->OBBIntersection(boxA.m_xmf3AABBCenter, boxA.m_xmf3AABBExtents, worldA,
			boxB.m_xmf3AABBCenter, boxB.m_xmf3AABBExtents, worldB));
	default:
		return false;
	}
}