#include "stdafx.h"
#include "Object.h"
#include "CollisionManager.h"

CCollisionManager::CCollisionManager()
{
	m_pd3dSphereBuffer = NULL;
	m_pd3dBoundingBoxBuffer = NULL;

	DebugSphereRender = false;
	DebugBoundingBoxRender = false;
}

CCollisionManager::~CCollisionManager()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [충돌 체크 알고리즘]

//=====[GJK Algorithm]==================================================================
XMFLOAT3 CCollisionManager::Support(const OBB& obb, const XMFLOAT3& dir)
{
	XMFLOAT3 result = obb.center;
	XMFLOAT3 dirLocal = dir;

	for (int i = 0; i < 3; ++i)
	{
		XMFLOAT3 axis = obb.axis[i];
		float    ext = (&obb.extent.x)[i];

		float sign = (Vector3::DotProduct(axis, dirLocal) > 0.0f) ? 1.0f : -1.0f;

		XMFLOAT3 offset = Vector3::ScalarProduct(axis, ext * sign, false);

		result = Vector3::Add(result, offset);
	}
	return result;
}

XMFLOAT3 CCollisionManager::Support(const OBB& A, const OBB& B, const XMFLOAT3& dir)
{
	XMFLOAT3 dirLocal = dir;
	XMFLOAT3 p1 = Support(A, dirLocal);

	XMFLOAT3 negDir = Vector3::ScalarProduct(dirLocal, -1.0f, false);
	XMFLOAT3 p2 = Support(B, negDir);

	return Vector3::Subtract(p1, p2);
}

bool CCollisionManager::HandleSimplex(std::vector<XMFLOAT3>& simplex, XMFLOAT3& dir)
{
	if (simplex.size() == 2)
	{
		XMFLOAT3 A = simplex[1];
		XMFLOAT3 B = simplex[0];
		XMFLOAT3 AB = Vector3::Subtract(B, A);
		XMFLOAT3 AO = Vector3::ScalarProduct(A, -1.0f, false);

		dir = Vector3::CrossProduct(Vector3::CrossProduct(AB, AO), AB);
	}
	else if (simplex.size() == 3)
	{
		XMFLOAT3 A = simplex[2];
		XMFLOAT3 B = simplex[1];
		XMFLOAT3 C = simplex[0];
		XMFLOAT3 AB = Vector3::Subtract(B, A);
		XMFLOAT3 AC = Vector3::Subtract(C, A);
		XMFLOAT3 AO = Vector3::ScalarProduct(A, -1.0f, false);

		XMFLOAT3 ABC = Vector3::CrossProduct(AB, AC);

		if (Vector3::DotProduct(Vector3::CrossProduct(ABC, AC), AO) > 0)
		{
			simplex.erase(simplex.begin() + 1);
			dir = Vector3::CrossProduct(AC, AO);
			dir = Vector3::CrossProduct(dir, AC);
		}
		else if (Vector3::DotProduct(Vector3::CrossProduct(AB, ABC), AO) > 0)
		{
			simplex.erase(simplex.begin());
			dir = Vector3::CrossProduct(AB, AO);
			dir = Vector3::CrossProduct(dir, AB);
		}
		else
		{
			if (Vector3::DotProduct(ABC, AO) > 0)
				dir = ABC;
			else
			{
				std::swap(simplex[0], simplex[1]);
				dir = Vector3::ScalarProduct(ABC, -1.0f, false);
			}
		}
	}
	else if (simplex.size() == 4)
	{
		XMFLOAT3 A = simplex[3];
		XMFLOAT3 B = simplex[2];
		XMFLOAT3 C = simplex[1];
		XMFLOAT3 D = simplex[0];
		XMFLOAT3 AO = Vector3::ScalarProduct(A, -1.0f, false);

		XMFLOAT3 AB = Vector3::Subtract(B, A);
		XMFLOAT3 AC = Vector3::Subtract(C, A);
		XMFLOAT3 AD = Vector3::Subtract(D, A);

		XMFLOAT3 ABC = Vector3::CrossProduct(AB, AC);
		XMFLOAT3 ACD = Vector3::CrossProduct(AC, AD);
		XMFLOAT3 ADB = Vector3::CrossProduct(AD, AB);

		if (Vector3::DotProduct(ABC, AO) > 0)
		{
			simplex.erase(simplex.begin());
			dir = ABC;
		}
		else if (Vector3::DotProduct(ACD, AO) > 0)
		{
			simplex.erase(simplex.begin() + 2);
			dir = ACD;
		}
		else if (Vector3::DotProduct(ADB, AO) > 0)
		{
			simplex.erase(simplex.begin() + 1);
			dir = ADB;
		}
		else
		{
			return true;
		}
	}
	return false;
}

bool CCollisionManager::Collision(const OBB& A, const OBB& B)
{
	std::vector<XMFLOAT3> simplex;
	XMFLOAT3 dir = XMFLOAT3(1.0f, 0.0f, 0.0f);

	simplex.push_back(Support(A, B, dir));
	dir = Vector3::ScalarProduct(simplex[0], -1.0f, false);

	for (int i = 0; i < 20; ++i)
	{
		XMFLOAT3 newPoint = Support(A, B, dir);
		if (Vector3::DotProduct(newPoint, dir) < 0.0f)
			return false;

		simplex.push_back(newPoint);

		if (HandleSimplex(simplex, dir))
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [충돌 정보 추가]
void CCollisionManager::AddCollisionInfo(CGameObject& gameObject, XMFLOAT4X4& xmf4x4ModelMatrix, bool Root)
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

void CCollisionManager::AddSphere(CGameObject& gameObject, float Radius, bool Root)
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

void CCollisionManager::AddBoundingBox(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const UINT Idx)
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

void CCollisionManager::UpdateCollisionSRV(ID3D12GraphicsCommandList* pd3dCommandList)
{
	UpdateSphereSRV(pd3dCommandList);
	UpdateBoundindBoxSRV(pd3dCommandList);
}

void CCollisionManager::UpdateSphereSRV(ID3D12GraphicsCommandList* pd3dCommandList)
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

void CCollisionManager::UpdateBoundindBoxSRV(ID3D12GraphicsCommandList* pd3dCommandList)
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
FLOAT CCollisionManager::CalculateRadiusSquared(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const XMFLOAT4X4& xmf4x4Model)
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
void CCollisionManager::CheckSphereCollision(ID3D12GraphicsCommandList* pd3dCommandList, std::vector<CGameObject*>& gameObjects)
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

		// 중심 계산 (월드행렬 기반)
		XMFLOAT3 centerA = XMFLOAT3(pObjA->m_xmf4x4World._41, pObjA->m_xmf4x4World._44, pObjA->m_xmf4x4World._43);

		for (size_t j = i + 1; j < count; ++j)
		{
			SRV_SPHERE_INFO& B = m_vSphereInfo[j];
			CGameObject* pObjB = gameObjects[B.m_objectIndex];
			if (!pObjB) continue;

			XMFLOAT3 centerB = XMFLOAT3(pObjB->m_xmf4x4World._41, pObjB->m_xmf4x4World._44, pObjB->m_xmf4x4World._43);

			// 거리 계산
			float dx = centerA.x - centerB.x;
			float dy = centerA.y - centerB.y;
			float dz = centerA.z - centerB.z;
			float distSq = dx * dx + dy * dy + dz * dz;
			float radiusSum = A.m_fRadius + B.m_fRadius;

			if (distSq <= radiusSum * radiusSum)
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
		CheckOBBCollisionPairs(pd3dCommandList, gameObjects, collisionSpherePairs);
	}
}

void CCollisionManager::CheckOBBCollisionPairs
(ID3D12GraphicsCommandList* pd3dCommandList, 
	std::vector<CGameObject*>& gameObjects,
	const std::vector<std::pair<UINT, UINT>>& pairs)
{
	bool update = false;

	for (const auto& pair : pairs)
	{
		CGameObject* objA = gameObjects[pair.first];
		CGameObject* objB = gameObjects[pair.second];

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

bool CCollisionManager::CheckOBBCollision
(const SRV_BOUNDINGBOX_INFO& boxA, 
	const XMFLOAT4X4& worldA, 
	const SRV_BOUNDINGBOX_INFO& boxB, 
	const XMFLOAT4X4& worldB)
{
	OBB A, B;

	XMFLOAT3 cA = boxA.m_xmf3AABBCenter;
	XMFLOAT3 cB = boxB.m_xmf3AABBCenter;

	A.center = Vector3::TransformCoord(cA, const_cast<XMFLOAT4X4&>(worldA));
	B.center = Vector3::TransformCoord(cB, const_cast<XMFLOAT4X4&>(worldB));


	A.axis[0] = Vector3::Normalize(XMFLOAT3(worldA._11, worldA._12, worldA._13));
	A.axis[1] = Vector3::Normalize(XMFLOAT3(worldA._21, worldA._22, worldA._23));
	A.axis[2] = Vector3::Normalize(XMFLOAT3(worldA._31, worldA._32, worldA._33));

	B.axis[0] = Vector3::Normalize(XMFLOAT3(worldB._11, worldB._12, worldB._13));
	B.axis[1] = Vector3::Normalize(XMFLOAT3(worldB._21, worldB._22, worldB._23));
	B.axis[2] = Vector3::Normalize(XMFLOAT3(worldB._31, worldB._32, worldB._33));

	A.extent = boxA.m_xmf3AABBExtents;
	B.extent = boxB.m_xmf3AABBExtents;

	return Collision(A, B);
}

