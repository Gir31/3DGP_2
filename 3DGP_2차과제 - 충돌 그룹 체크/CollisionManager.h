#pragma once
#include "CollisionAlgorithm.h"

#define MAKE_PAIR(a, b) ((min(a, b) << 16) | max(a, b))

#define CG_DYNAMIC (CG_PLAYER | CG_ENEMY | CG_ITEM | CG_PROJECTILE)
#define NONE_CHECK (CG_NONE | CG_IGNORE)
#define ALL_STATIC CG_STATIC

#define PAIR_PLAYER_STATIC MAKE_PAIR(CG_PLAYER, CG_STATIC)
#define PAIR_PLAYER_ENEMY MAKE_PAIR(CG_PLAYER, CG_ENEMY)
#define PAIR_PLAYER_ITEM MAKE_PAIR(CG_PLAYER, CG_ITEM)
#define PAIR_PLAYER_PROJECTILE MAKE_PAIR(CG_PLAYER, CG_PROJECTILE)
#define PAIR_ENEMY_STATIC MAKE_PAIR(CG_ENEMY, CG_STATIC)
#define PAIR_ENEMY_ENEMY MAKE_PAIR(CG_ENEMY, CG_ENEMY)
#define PAIR_ENEMY_PROJECTILE MAKE_PAIR(CG_ENEMY, CG_PROJECTILE)

enum {
	BOUNDING_SPHERE_SHADER,
	BOUNDING_BOX_SHADER
};


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

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

public:
	//==[디버깅 변수]==============
	int												m_nDebugShaders = 0;
	CShader											**m_ppDebugShaders = NULL;

	//==[GPU 리소스]================
	ID3D12Resource*									m_pd3dSphereBuffer = NULL;
	ID3D12Resource*									m_pd3dBoundingBoxBuffer = NULL;

	//==[충돌 체크 정보]============
	std::vector<SRV_SPHERE_INFO>					m_vSphereInfo;
	std::vector<SRV_BOUNDINGBOX_INFO>				m_vBoundingBoxInfo;

	//==[디버깅 렌더 플래그]========
	bool											DebugSphereRender = false;
	bool											DebugBoundingBoxRender = false;

	//==[충돌 알고리즘]=============
	SPHERE_SPHERE_TEST*								SST = NULL;
	GILBERT_JOHNSON_KEERTHI*						GJK = NULL;
	SEPARATING_AXIS_THEOREM*						SAT = NULL;

public:
	//==[디버깅 렌더링]=============
	void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
	void ReleaseObject();
	void ReleaseUploadBuffers();
	void ReleaseShaderVariables();

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
	bool CheckCollisionGroup(const SRV_BOUNDINGBOX_INFO& boxA, const XMFLOAT4X4& worldA, const UINT& collisionGroupA,
		const SRV_BOUNDINGBOX_INFO& boxB, const XMFLOAT4X4& worldB, const UINT& collisionGroupB);
};