#pragma once

struct SPHERE
{
	XMFLOAT3 xmf3Center;
	FLOAT fRadius;
};

struct OBB
{
	XMFLOAT3 xmf3Center;
	XMFLOAT3 xmf3Axis[3];  // Right, Up, Look (정규화됨)
	XMFLOAT3 xmf3Extent;   // x, y, z 반경
};

class SPHERE_SPHERE_TEST
{
public:
	SPHERE_SPHERE_TEST() {}
	~SPHERE_SPHERE_TEST() {}

public:
	bool SphereIntersection(const FLOAT& radiusA, const XMFLOAT4X4& worldA, 
		const FLOAT& radiusB, const XMFLOAT4X4& worldB);
	bool Intersection(const SPHERE& A, const SPHERE& B);
};

class GILBERT_JOHNSON_KEERTHI
{
public:
	GILBERT_JOHNSON_KEERTHI() {}
	~GILBERT_JOHNSON_KEERTHI() {}

public:
	bool OBBIntersection(const XMFLOAT3& centerA, const XMFLOAT3& extentA, const XMFLOAT4X4& worldA,
		const XMFLOAT3& centerB, const XMFLOAT3& extentB, const XMFLOAT4X4& worldB);
	bool Intersection(const OBB& A, const OBB& B);
	bool HandleSimplex(std::vector<XMFLOAT3>& simplex, XMFLOAT3& dir);
	XMFLOAT3 Support(const OBB& obb, const XMFLOAT3& dir);
	XMFLOAT3 Support(const OBB& A, const OBB& B, const XMFLOAT3& dir);
};

class SEPARATING_AXIS_THEOREM
{
public:
	SEPARATING_AXIS_THEOREM() {}
	~SEPARATING_AXIS_THEOREM() {}

public:
	bool OBBIntersection(const XMFLOAT3& centerA, const XMFLOAT3& extentA, const XMFLOAT4X4& worldA,
		const XMFLOAT3& centerB, const XMFLOAT3& extentB, const XMFLOAT4X4& worldB);
	bool Intersection(const OBB& A, const OBB& B);
};