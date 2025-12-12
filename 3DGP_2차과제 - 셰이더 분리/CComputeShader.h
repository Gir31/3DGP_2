#pragma once
#include "ShaderBase.h"

class CComputeShader : public CShader
{
public:
	CComputeShader();
	virtual ~CComputeShader();

	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature, WCHAR* pszFileName, LPCSTR pszShaderName);
	virtual void Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, int x, int y, int z);
};