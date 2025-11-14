#include "stdafx.h"
#include "CollisionAlgorithm.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [구-구 충돌 체크 알고리즘]

bool SPHERE_SPHERE_TEST::SphereIntersection(const FLOAT& radiusA, const XMFLOAT4X4& worldA,
	const FLOAT& radiusB, const XMFLOAT4X4& worldB)
{
	SPHERE A, B;

	A.xmf3Center = XMFLOAT3(worldA._41, worldA._42, worldA._43);
	A.fRadius = radiusA;
	B.xmf3Center = XMFLOAT3(worldB._41, worldB._42, worldB._43);
	B.fRadius = radiusB;

	return Intersection(A, B);
}

bool SPHERE_SPHERE_TEST::Intersection(const SPHERE& A, const SPHERE& B)
{
	float dx = A.xmf3Center.x - B.xmf3Center.x;
	float dy = A.xmf3Center.y - B.xmf3Center.y;
	float dz = A.xmf3Center.z - B.xmf3Center.z;
	float distance = dx * dx + dy * dy + dz * dz;
	float RadiusSum = A.fRadius + B.fRadius;

	RadiusSum *= RadiusSum;
	
	return (distance <= RadiusSum);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [GJK 거리 알고리즘]

bool GILBERT_JOHNSON_KEERTHI::OBBIntersection(const XMFLOAT3& centerA, const XMFLOAT3& extentA, const XMFLOAT4X4& worldA,
	const XMFLOAT3& centerB, const XMFLOAT3& extentB, const XMFLOAT4X4& worldB)
{
	OBB A, B;

	A.xmf3Center = Vector3::TransformCoord(centerA, worldA);
	B.xmf3Center = Vector3::TransformCoord(centerB, worldB);

	A.xmf3Axis[0] = Vector3::Normalize(XMFLOAT3(worldA._11, worldA._12, worldA._13));
	A.xmf3Axis[1] = Vector3::Normalize(XMFLOAT3(worldA._21, worldA._22, worldA._23));
	A.xmf3Axis[2] = Vector3::Normalize(XMFLOAT3(worldA._31, worldA._32, worldA._33));

	B.xmf3Axis[0] = Vector3::Normalize(XMFLOAT3(worldB._11, worldB._12, worldB._13));
	B.xmf3Axis[1] = Vector3::Normalize(XMFLOAT3(worldB._21, worldB._22, worldB._23));
	B.xmf3Axis[2] = Vector3::Normalize(XMFLOAT3(worldB._31, worldB._32, worldB._33));

	A.xmf3Extent = extentA;
	B.xmf3Extent = extentB;

	return Intersection(A, B);
}

bool GILBERT_JOHNSON_KEERTHI::Intersection(const OBB& A, const OBB& B)
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

bool GILBERT_JOHNSON_KEERTHI::HandleSimplex(std::vector<XMFLOAT3>& simplex, XMFLOAT3& dir)
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

XMFLOAT3 GILBERT_JOHNSON_KEERTHI::Support(const OBB& A, const OBB& B, const XMFLOAT3& dir)
{
	XMFLOAT3 dirLocal = dir;
	XMFLOAT3 p1 = Support(A, dirLocal);

	XMFLOAT3 negDir = Vector3::ScalarProduct(dirLocal, -1.0f, false);
	XMFLOAT3 p2 = Support(B, negDir);

	return Vector3::Subtract(p1, p2);
}

XMFLOAT3 GILBERT_JOHNSON_KEERTHI::Support(const OBB& obb, const XMFLOAT3& dir)
{
	XMFLOAT3 result = obb.xmf3Center;
	XMFLOAT3 dirLocal = dir;

	for (int i = 0; i < 3; ++i)
	{
		XMFLOAT3 axis = obb.xmf3Axis[i];
		float    ext = (&obb.xmf3Extent.x)[i];

		float sign = (Vector3::DotProduct(axis, dirLocal) > 0.0f) ? 1.0f : -1.0f;

		XMFLOAT3 offset = Vector3::ScalarProduct(axis, ext * sign, false);

		result = Vector3::Add(result, offset);
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [SAT 알고리즘]

bool SEPARATING_AXIS_THEOREM::OBBIntersection(const XMFLOAT3& centerA, const XMFLOAT3& extentA, const XMFLOAT4X4& worldA,
	const XMFLOAT3& centerB, const XMFLOAT3& extentB, const XMFLOAT4X4& worldB)
{
	OBB A, B;

	// center
	A.xmf3Center = Vector3::TransformCoord(centerA, worldA);
	B.xmf3Center = Vector3::TransformCoord(centerB, worldB);

	// axis
	A.xmf3Axis[0] = Vector3::Normalize(XMFLOAT3(worldA._11, worldA._12, worldA._13));
	A.xmf3Axis[1] = Vector3::Normalize(XMFLOAT3(worldA._21, worldA._22, worldA._23));
	A.xmf3Axis[2] = Vector3::Normalize(XMFLOAT3(worldA._31, worldA._32, worldA._33));

	B.xmf3Axis[0] = Vector3::Normalize(XMFLOAT3(worldB._11, worldB._12, worldB._13));
	B.xmf3Axis[1] = Vector3::Normalize(XMFLOAT3(worldB._21, worldB._22, worldB._23));
	B.xmf3Axis[2] = Vector3::Normalize(XMFLOAT3(worldB._31, worldB._32, worldB._33));

	// scale (중요)
	float A_scaleX = Vector3::Length(XMFLOAT3(worldA._11, worldA._12, worldA._13));
	float A_scaleY = Vector3::Length(XMFLOAT3(worldA._21, worldA._22, worldA._23));
	float A_scaleZ = Vector3::Length(XMFLOAT3(worldA._31, worldA._32, worldA._33));

	float B_scaleX = Vector3::Length(XMFLOAT3(worldB._11, worldB._12, worldB._13));
	float B_scaleY = Vector3::Length(XMFLOAT3(worldB._21, worldB._22, worldB._23));
	float B_scaleZ = Vector3::Length(XMFLOAT3(worldB._31, worldB._32, worldB._33));

	// extent
	A.xmf3Extent.x = extentA.x * A_scaleX;
	A.xmf3Extent.y = extentA.y * A_scaleY;
	A.xmf3Extent.z = extentA.z * A_scaleZ;

	B.xmf3Extent.x = extentB.x * B_scaleX;
	B.xmf3Extent.y = extentB.y * B_scaleY;
	B.xmf3Extent.z = extentB.z * B_scaleZ;


	return Intersection(A, B);
}

bool SEPARATING_AXIS_THEOREM::Intersection(const OBB& A, const OBB& B)
{
	// 축을 조금 더 편하게 쓰기 위해 로컬 배열로 복사
	XMFLOAT3 Aaxis[3] = { A.xmf3Axis[0], A.xmf3Axis[1], A.xmf3Axis[2] };
	XMFLOAT3 Baxis[3] = { B.xmf3Axis[0], B.xmf3Axis[1], B.xmf3Axis[2] };

	// R[i][j] = A축 i 와 B축 j 의 cos(θ)
	float R[3][3];
	float AbsR[3][3];

	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			R[i][j] = Vector3::DotF3(Aaxis[i], Baxis[j]);
			AbsR[i][j] = fabsf(R[i][j]) + EPSILON; // float 오차 보정
		}
	}

	// B.center - A.center 를 A의 좌표계로 변환
	XMFLOAT3 tWorld = XMFLOAT3(
		B.xmf3Center.x - A.xmf3Center.x,
		B.xmf3Center.y - A.xmf3Center.y,
		B.xmf3Center.z - A.xmf3Center.z);

	float t[3] = {
		Vector3::DotF3(tWorld, Aaxis[0]),
		Vector3::DotF3(tWorld, Aaxis[1]),
		Vector3::DotF3(tWorld, Aaxis[2])
	};

	float ra, rb;

	// 1) A의 세 축(A0, A1, A2)에 대한 분리축 검사
	for (int i = 0; i < 3; ++i)
	{
		ra = (&A.xmf3Extent.x)[i];
		rb =
			(&B.xmf3Extent.x)[0] * AbsR[i][0] +
			(&B.xmf3Extent.x)[1] * AbsR[i][1] +
			(&B.xmf3Extent.x)[2] * AbsR[i][2];

		if (fabsf(t[i]) > ra + rb) return false;
	}

	// 2) B의 세 축(B0, B1, B2)에 대한 분리축 검사
	for (int j = 0; j < 3; ++j)
	{
		ra =
			(&A.xmf3Extent.x)[0] * AbsR[0][j] +
			(&A.xmf3Extent.x)[1] * AbsR[1][j] +
			(&A.xmf3Extent.x)[2] * AbsR[2][j];
		rb = (&B.xmf3Extent.x)[j];

		float tProj =
			t[0] * R[0][j] +
			t[1] * R[1][j] +
			t[2] * R[2][j];

		if (fabsf(tProj) > ra + rb) return false;
	}

	// 3) 9개의 교차축(Ai × Bj)에 대한 검사
	// A0 x B0
	ra = (&A.xmf3Extent.x)[1] * AbsR[2][0] + (&A.xmf3Extent.x)[2] * AbsR[1][0];
	rb = (&B.xmf3Extent.x)[1] * AbsR[0][2] + (&B.xmf3Extent.x)[2] * AbsR[0][1];
	if (fabsf(t[2] * R[1][0] - t[1] * R[2][0]) > ra + rb) return false;

	// A0 x B1
	ra = (&A.xmf3Extent.x)[1] * AbsR[2][1] + (&A.xmf3Extent.x)[2] * AbsR[1][1];
	rb = (&B.xmf3Extent.x)[0] * AbsR[0][2] + (&B.xmf3Extent.x)[2] * AbsR[0][0];
	if (fabsf(t[2] * R[1][1] - t[1] * R[2][1]) > ra + rb) return false;

	// A0 x B2
	ra = (&A.xmf3Extent.x)[1] * AbsR[2][2] + (&A.xmf3Extent.x)[2] * AbsR[1][2];
	rb = (&B.xmf3Extent.x)[0] * AbsR[0][1] + (&B.xmf3Extent.x)[1] * AbsR[0][0];
	if (fabsf(t[2] * R[1][2] - t[1] * R[2][2]) > ra + rb) return false;

	// A1 x B0
	ra = (&A.xmf3Extent.x)[0] * AbsR[2][0] + (&A.xmf3Extent.x)[2] * AbsR[0][0];
	rb = (&B.xmf3Extent.x)[1] * AbsR[1][2] + (&B.xmf3Extent.x)[2] * AbsR[1][1];
	if (fabsf(t[0] * R[2][0] - t[2] * R[0][0]) > ra + rb) return false;

	// A1 x B1
	ra = (&A.xmf3Extent.x)[0] * AbsR[2][1] + (&A.xmf3Extent.x)[2] * AbsR[0][1];
	rb = (&B.xmf3Extent.x)[0] * AbsR[1][2] + (&B.xmf3Extent.x)[2] * AbsR[1][0];
	if (fabsf(t[0] * R[2][1] - t[2] * R[0][1]) > ra + rb) return false;

	// A1 x B2
	ra = (&A.xmf3Extent.x)[0] * AbsR[2][2] + (&A.xmf3Extent.x)[2] * AbsR[0][2];
	rb = (&B.xmf3Extent.x)[0] * AbsR[1][1] + (&B.xmf3Extent.x)[1] * AbsR[1][0];
	if (fabsf(t[0] * R[2][2] - t[2] * R[0][2]) > ra + rb) return false;

	// A2 x B0
	ra = (&A.xmf3Extent.x)[0] * AbsR[1][0] + (&A.xmf3Extent.x)[1] * AbsR[0][0];
	rb = (&B.xmf3Extent.x)[1] * AbsR[2][2] + (&B.xmf3Extent.x)[2] * AbsR[2][1];
	if (fabsf(t[1] * R[0][0] - t[0] * R[1][0]) > ra + rb) return false;

	// A2 x B1
	ra = (&A.xmf3Extent.x)[0] * AbsR[1][1] + (&A.xmf3Extent.x)[1] * AbsR[0][1];
	rb = (&B.xmf3Extent.x)[0] * AbsR[2][2] + (&B.xmf3Extent.x)[2] * AbsR[2][0];
	if (fabsf(t[1] * R[0][1] - t[0] * R[1][1]) > ra + rb) return false;

	// A2 x B2
	ra = (&A.xmf3Extent.x)[0] * AbsR[1][2] + (&A.xmf3Extent.x)[1] * AbsR[0][2];
	rb = (&B.xmf3Extent.x)[0] * AbsR[2][1] + (&B.xmf3Extent.x)[1] * AbsR[2][0];
	if (fabsf(t[1] * R[0][2] - t[0] * R[1][2]) > ra + rb) return false;

	// 15개 축 전부 통과 → 분리축 없음 → 충돌
	return true;
}