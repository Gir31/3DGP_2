#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CComputeShader.h"

CComputeShader::CComputeShader()
{
}

CComputeShader::~CComputeShader()
{
}

void CComputeShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dRootSignature, WCHAR* pszFileName, LPCSTR pszShaderName)
{
	if (m_ppd3dPipelineStates)
	{
		// 혹시 이미 있다면 해제
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}

	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates]; // <--- 필수!


	ID3DBlob* pComputeShaderBlob = NULL;
	ID3DBlob* pErrorBlob = NULL;

	// 1. 쉐이더 컴파일 및 에러 확인
	HRESULT hr = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, "cs_5_1", 0, 0, &pComputeShaderBlob, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
			pErrorBlob->Release();
		}
		return; // 컴파일 실패 시 리턴
	}

	if (pErrorBlob) pErrorBlob->Release();

	// 2. PSO 생성
	D3D12_COMPUTE_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dRootSignature;
	d3dPipelineStateDesc.CS.pShaderBytecode = pComputeShaderBlob->GetBufferPointer();
	d3dPipelineStateDesc.CS.BytecodeLength = pComputeShaderBlob->GetBufferSize();
	d3dPipelineStateDesc.NodeMask = 0;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// [중요 2] 생성 결과 확인
	hr = pd3dDevice->CreateComputePipelineState(&d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Compute Pipeline State!\n");
		m_ppd3dPipelineStates[0] = NULL;
	}

	if (pComputeShaderBlob) pComputeShaderBlob->Release();
}

void CComputeShader::Dispatch(ID3D12GraphicsCommandList* pd3dCommandList, int x, int y, int z)
{
	if (m_ppd3dPipelineStates && m_ppd3dPipelineStates[0])
	{
		pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
		pd3dCommandList->Dispatch(x, y, z);
	}
	else
	{
		// 디버깅용: PSO가 없다는 것을 알림
		OutputDebugString(L"Compute Shader Dispatch Failed: PSO is NULL\n");
	}
}