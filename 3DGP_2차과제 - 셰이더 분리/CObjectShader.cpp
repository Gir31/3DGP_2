#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CObjectShader.h"

CObjectsShader::CObjectsShader()
{
	m_nGameObjects = 100;
}

CObjectsShader::~CObjectsShader()
{
}

float Random(float fMin, float fMax)
{
	float fRandomValue = (float)rand();
	if (fRandomValue < fMin) fRandomValue = fMin;
	if (fRandomValue > fMax) fRandomValue = fMax;
	return(fRandomValue);
}

float Random()
{
	return(rand() / float(RAND_MAX));
}

XMFLOAT3 RandomPositionInSphere(XMFLOAT3 xmf3Center, float fRadius, int nColumn, int nColumnSpace)
{
	float fAngle = Random() * 360.0f * (2.0f * 3.14159f / 360.0f);

	XMFLOAT3 xmf3Position;
	xmf3Position.x = xmf3Center.x + fRadius * sin(fAngle);
	xmf3Position.y = xmf3Center.y - (nColumn * float(nColumnSpace) / 2.0f) + (nColumn * nColumnSpace) + Random();
	xmf3Position.z = xmf3Center.z + fRadius * cos(fAngle);

	return(xmf3Position);
}

void CObjectsShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CEnemyShader::CEnemyShader()
{
	m_nGameObjects = 100;
}

CEnemyShader::~CEnemyShader()
{
}

void CEnemyShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_ppGameObject = new CGameObject * [m_nGameObjects];

	CGameObject* pMi24Model = CGameObject::LoadGeometryFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Mi24.bin", this);

	int nColumnSpace = 5, nColumnSize = 30;
	int nFirstPassColumnSize = (m_nGameObjects % nColumnSize) > 0 ? (nColumnSize - 1) : nColumnSize;

	int nObjects = 0;
	for (int h = 0; h < nFirstPassColumnSize; h++)
	{
		for (int i = 0; i < floor(float(m_nGameObjects) / float(nColumnSize)); i++)
		{
			if (nObjects % 2)
			{
				m_ppGameObject[nObjects] = new CMi24Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppGameObject[nObjects]->SetChild(pMi24Model->Clone());
			}
			else
			{
				m_ppGameObject[nObjects] = new CMi24Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppGameObject[nObjects]->SetChild(pMi24Model->Clone());
			}
			XMFLOAT3 xmf3RandomPosition = RandomPositionInSphere(XMFLOAT3(920.0f, 0.0f, 900.0f), Random(20.0f, 150.0f), h - int(floor(nColumnSize / 2.0f)), nColumnSpace);
			m_ppGameObject[nObjects]->SetPosition(xmf3RandomPosition.x, xmf3RandomPosition.y + 750.0f, xmf3RandomPosition.z);
			m_ppGameObject[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppGameObject[nObjects]->m_CollisionGroup = CG_ENEMY;
			m_ppGameObject[nObjects++]->PrepareAnimate();
		}
	}

	if (nFirstPassColumnSize != nColumnSize)
	{
		for (int i = 0; i < m_nGameObjects - int(floor(float(m_nGameObjects) / float(nColumnSize)) * nFirstPassColumnSize); i++)
		{
			if (nObjects % 2)
			{
				m_ppGameObject[nObjects] = new CMi24Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppGameObject[nObjects]->SetChild(pMi24Model->Clone());
			}
			else
			{
				m_ppGameObject[nObjects] = new CMi24Object(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
				m_ppGameObject[nObjects]->SetChild(pMi24Model->Clone());
			}
			XMFLOAT3 xmf3RandomPosition = RandomPositionInSphere(XMFLOAT3(920.0f, 0.0f, 1200.0f), Random(20.0f, 150.0f), nColumnSize - int(floor(nColumnSize / 2.0f)), nColumnSpace);
			m_ppGameObject[nObjects]->SetPosition(xmf3RandomPosition.x, xmf3RandomPosition.y + 850.0f, xmf3RandomPosition.z);
			m_ppGameObject[nObjects]->Rotate(0.0f, 90.0f, 0.0f);
			m_ppGameObject[nObjects]->m_CollisionGroup = CG_ENEMY;
			m_ppGameObject[nObjects++]->PrepareAnimate();
		}
	}


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CEnemyShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i] && m_ppGameObject[i]->m_bVisible && !m_ppGameObject[i]->die) m_ppGameObject[i]->Render(pd3dCommandList, pCamera);
	}
}