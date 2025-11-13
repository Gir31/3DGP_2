#pragma once
#include "CollisionAlgorithm.h"

struct SRV_SPHERE_INFO
{
	FLOAT 							m_fRadius;
	UINT 							m_objectIndex;
	BOOL							m_collision;
};

struct SRV_BOUNDINGBOX_INFO
{
	XMFLOAT3						m_xmf3AABBCenter;
	XMFLOAT3						m_xmf3AABBExtents;
	UINT 							m_objectIndex;
	BOOL							m_collision;
};

class CCollisionManager
{
public:
	CCollisionManager();
	~CCollisionManager();

public:
	//==[GPU 리소스]================
	ID3D12Resource*									m_pd3dSphereBuffer = NULL;
	ID3D12Resource*									m_pd3dBoundingBoxBuffer = NULL;

	//==[충돌 체크 정보]============
	std::vector<SRV_SPHERE_INFO>					m_vSphereInfo;
	std::vector<SRV_BOUNDINGBOX_INFO>				m_vBoundingBoxInfo;

	//==[디버깅 렌더]===============
	bool											DebugSphereRender = false;
	bool											DebugBoundingBoxRender = false;

	//==[충돌 알고리즘]=============
	SPHERE_SPHERE_TEST*								SST = NULL;
	GILBERT_JOHNSON_KEERTHI*						GJK = NULL;

public:
	//==[충돌 정보 추가]============
	void AddCollisionInfo(CGameObject& gameObject, XMFLOAT4X4& xmf4x4ModelMatrix, bool Root);
	void AddSphere(CGameObject& gameObject, float Radius, bool Root);
	void AddBoundingBox(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const UINT Idx);

	//==[충돌 정보 업데이트]========
	void UpdateCollisionSRV(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateSphereSRV(ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateBoundindBoxSRV(ID3D12GraphicsCommandList* pd3dCommandList);

	//==[기타 정보]=================
	FLOAT CalculateRadiusSquared(const XMFLOAT3& xmf3MeshCenter, const XMFLOAT3& xmf3MeshExtents, const XMFLOAT4X4& xmf4x4Model);

	//==[충돌 체크]=================
	void CheckSphereCollision(ID3D12GraphicsCommandList* pd3dCommandList, std::vector<CGameObject*>& gameObjects);
	void CheckOBBCollisionPairs(ID3D12GraphicsCommandList* pd3dCommandList, std::vector<CGameObject*>& gameObjects, const std::vector<std::pair<UINT, UINT>>& pairs);
};