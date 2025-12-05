//-----------------------------------------------------------------------------
// File: Shader.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Shader.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"

CShader::CShader()
{
}

CShader::~CShader()
{
	ReleaseShaderVariables();

	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++) if (m_ppd3dPipelineStates[i]) m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateHullShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateDomainShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreateGeometryShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader()
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob *pd3dErrorBlob = NULL;
	HRESULT hResult = ::D3DCompileFromFile(pszFileName, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, &pd3dErrorBlob);
	
	if (FAILED(hResult) || !(*ppd3dShaderBlob))
	{
		if (pd3dErrorBlob)
		{
			OutputDebugStringA((char*)pd3dErrorBlob->GetBufferPointer());
			pd3dErrorBlob->Release();
		}
		else
		{
			wchar_t msg[512];
			swprintf_s(msg, L"[Shader Compile Error] %s (%S)\n", pszFileName, pszShaderName);
			OutputDebugStringW(msg);
		}

		D3D12_SHADER_BYTECODE empty = {};
		empty.BytecodeLength = 0;
		empty.pShaderBytecode = nullptr;
		return empty;
	}

	char *pErrorString = NULL;
	if (pd3dErrorBlob) pErrorString = (char *)pd3dErrorBlob->GetBufferPointer();

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();

	return(d3dShaderByteCode);
}

#define _WITH_WFOPEN
//#define _WITH_STD_STREAM

#ifdef _WITH_STD_STREAM
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#endif

D3D12_SHADER_BYTECODE CShader::ReadCompiledShaderFromFile(WCHAR *pszFileName, ID3DBlob **ppd3dShaderBlob)
{
	UINT nReadBytes = 0;
#ifdef _WITH_WFOPEN
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pszFileName, L"rb");
	::fseek(pFile, 0, SEEK_END);
	int nFileSize = ::ftell(pFile);
	BYTE *pByteCode = new BYTE[nFileSize];
	::rewind(pFile);
	nReadBytes = (UINT)::fread(pByteCode, sizeof(BYTE), nFileSize, pFile);
	::fclose(pFile);
#endif
#ifdef _WITH_STD_STREAM
	std::ifstream ifsFile;
	ifsFile.open(pszFileName, std::ios::in | std::ios::ate | std::ios::binary);
	nReadBytes = (int)ifsFile.tellg();
	BYTE *pByteCode = new BYTE[*pnReadBytes];
	ifsFile.seekg(0);
	ifsFile.read((char *)pByteCode, nReadBytes);
	ifsFile.close();
#endif

	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	if (ppd3dShaderBlob)
	{
		*ppd3dShaderBlob = NULL;
		HRESULT hResult = D3DCreateBlob(nReadBytes, ppd3dShaderBlob);
		memcpy((*ppd3dShaderBlob)->GetBufferPointer(), pByteCode, nReadBytes);
		d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
		d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	}
	else
	{
		d3dShaderByteCode.BytecodeLength = nReadBytes;
		d3dShaderByteCode.pShaderBytecode = pByteCode;
	}

	return(d3dShaderByteCode);
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));
	//	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(d3dRasterizerDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(d3dDepthStencilDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(d3dBlendDesc);
}

void CShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList, int nPipelineState)
{
	if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[nPipelineState]);
}


void CShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i] && m_ppGameObject[i]->m_bVisible) m_ppGameObject[i]->Render(pd3dCommandList, pCamera);
	}
}

void CShader::ReleaseUploadBuffers()
{
	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i]) m_ppGameObject[i]->ReleaseUploadBuffers();
	}
}

void CShader::AnimateObjects(float fTimeElapsed) 
{
	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i]) 
		{
			m_ppGameObject[i]->Animate(fTimeElapsed);
			m_ppGameObject[i]->UpdateTransform(NULL);
		}
	}
}

void CShader::ReleaseObjects() 
{
	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i])
		{
			m_ppGameObject[i]->Release();
		}
	}
	if (m_ppGameObject) delete[] m_ppGameObject;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSkyBoxShader::CSkyBoxShader()
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 1;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc = {};
	d3dDepthStencilDesc.DepthEnable = FALSE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0xff;
	d3dDepthStencilDesc.StencilWriteMask = 0xff;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"SkyBox.hlsl", "VSSkyBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"SkyBox.hlsl", "PSSkyBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CSkyBoxShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	// ─────────────────────────────────────
	// 1) 공통 파이프라인 desc 기본 세팅
	// ─────────────────────────────────────
	ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// ─────────────────────────────────────
	// 1-1) 기본용 DepthStencil (전체 화면용, 깊이 off, 스텐실 off)
	// ─────────────────────────────────────
	{
		D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc = {};
		d3dDepthStencilDesc.DepthEnable = FALSE;
		d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
		d3dDepthStencilDesc.StencilEnable = FALSE;
		d3dDepthStencilDesc.StencilReadMask = 0xff;
		d3dDepthStencilDesc.StencilWriteMask = 0xff;
		d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
		d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
		d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		m_d3dPipelineStateDesc.DepthStencilState = d3dDepthStencilDesc;

		HRESULT hr = pd3dDevice->CreateGraphicsPipelineState(
			&m_d3dPipelineStateDesc,
			__uuidof(ID3D12PipelineState),
			(void**)&m_ppd3dPipelineStates[0]);

		if (FAILED(hr))
			OutputDebugString(L"[SkyBox] PSO[0] Create FAILED!!\n");
	}

	// ─────────────────────────────────────
	// 1-2) 거울반사용 DepthStencil (스텐실 = 1 픽셀에서만 그리기)
	// ─────────────────────────────────────
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC reflection = m_d3dPipelineStateDesc;

		// 깊이는 여전히 사용 안 함 (항상 배경)
		reflection.DepthStencilState.DepthEnable = FALSE;
		reflection.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		reflection.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;

		// 스텐실: mirror pass가 만든 값(1)에서만 통과
		reflection.DepthStencilState.StencilEnable = TRUE;
		reflection.DepthStencilState.StencilReadMask = 0xFF;
		reflection.DepthStencilState.StencilWriteMask = 0x00;

		reflection.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
		reflection.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		reflection.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		reflection.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

		reflection.DepthStencilState.BackFace = reflection.DepthStencilState.FrontFace;

		// 큐브 안쪽에서 보니까 front cull
		reflection.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

		HRESULT hr = pd3dDevice->CreateGraphicsPipelineState(
			&reflection,
			__uuidof(ID3D12PipelineState),
			(void**)&m_ppd3dPipelineStates[1]);

		if (FAILED(hr))
			OutputDebugString(L"[SkyBox] PSO[1] Reflection Create FAILED!!\n");
	}

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}


void CSkyBoxShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 1;

	m_ppGameObject = new CGameObject * [m_nGameObjects];

	m_ppGameObject[0] = new CSkyBox(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppGameObject[0]->m_CollisionGroup = CG_IGNORE;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CStandardShader::CStandardShader()
{
}

CStandardShader::~CStandardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CStandardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStandardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Standard.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CStandardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Standard.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CStandardShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);

	D3D12_DEPTH_STENCIL_DESC d = {};
	d.DepthEnable = TRUE;
	d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	d.StencilEnable = TRUE;
	d.StencilReadMask = 0xFF;
	d.StencilWriteMask = 0x00; 

	d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	d.BackFace = d.FrontFace;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC reflectionDesc = m_d3dPipelineStateDesc;
	reflectionDesc.DepthStencilState = d;

	reflectionDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&reflectionDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);


	if (FAILED(hResult))
		OutputDebugString(L"[Reflection PSO] STANDARD FAILED!!!!\n");

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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

void CObjectsShader::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
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


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CPlayerShader::CPlayerShader()
{
}

CPlayerShader::~CPlayerShader()
{
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 5;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 4, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Standard.hlsl", "VSStandard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Standard.hlsl", "PSStandard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CPlayerShader::CreateShader(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_BLEND_DESC d3dBlendFactorDesc = CShader::CreateBlendState();
	d3dBlendFactorDesc.RenderTarget[0].BlendEnable = TRUE;

	d3dBlendFactorDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_BLEND_FACTOR;
	d3dBlendFactorDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_BLEND_FACTOR;
	d3dBlendFactorDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	d3dBlendFactorDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_BLEND_FACTOR;
	d3dBlendFactorDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_BLEND_FACTOR;
	d3dBlendFactorDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.BlendState = d3dBlendFactorDesc; // 적용

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);


	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CTerrainShader::CTerrainShader()
{
}

CTerrainShader::~CTerrainShader()
{
}

D3D12_INPUT_LAYOUT_DESC CTerrainShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Terrain.hlsl", "VSTerrain", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateHullShader()
{
	return(CShader::CompileShaderFromFile(L"Terrain.hlsl", "HSTerrain", "hs_5_1", &m_pd3dHullShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreateDomainShader()
{
	return(CShader::CompileShaderFromFile(L"Terrain.hlsl", "DSTerrain", "ds_5_1", &m_pd3dDomainShaderBlob));
}

D3D12_SHADER_BYTECODE CTerrainShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Terrain.hlsl", "PSTerrain", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CTerrainShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.HS = CreateHullShader();
	m_d3dPipelineStateDesc.DS = CreateDomainShader(); 
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, 
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	// [수정] CTerrainShader::CreateShader()의 d 설정 부분
	D3D12_DEPTH_STENCIL_DESC d = {};
	d.DepthEnable = TRUE;
	d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	d.StencilEnable = TRUE;
	d.StencilReadMask = 0xFF;
	d.StencilWriteMask = 0x00;

	d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	d.BackFace = d.FrontFace;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC reflectionDesc = m_d3dPipelineStateDesc;
	reflectionDesc.DepthStencilState = d;
	reflectionDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	hResult = pd3dDevice->CreateGraphicsPipelineState(&reflectionDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);


	if (FAILED(hResult))
		OutputDebugString(L"[Reflection PSO] TERRAIN FAILED!!!!\n");

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release(); 
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CTerrainShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	m_nGameObjects = 1;

	m_ppGameObject = new CGameObject * [m_nGameObjects]; 

	m_ppGameObject[0] = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature,
		_T("Terrain/HeightMap.raw"), 257, 257, 257, 257, xmf3Scale, xmf4Color);
	m_ppGameObject[0]->m_CollisionGroup = CG_IGNORE;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBillboardShader::CBillboardShader()
{
}

CBillboardShader::~CBillboardShader()
{
}

D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CBillboardShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Billboard.hlsl", "VSBillboard", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardShader::CreateGeometryShader()
{
	return(CShader::CompileShaderFromFile(L"Billboard.hlsl", "GSBillboard", "gs_5_1", &m_pd3dGeometryShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Billboard.hlsl", "PSBillboard", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CBillboardShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.GS = CreateGeometryShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	// [수정] CBillboardShader::CreateShader()의 d 설정
	D3D12_DEPTH_STENCIL_DESC d = {};
	d.DepthEnable = TRUE;
	d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	d.StencilEnable = TRUE;
	d.StencilReadMask = 0xFF;
	d.StencilWriteMask = 0x00;

	d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	d.BackFace = d.FrontFace;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC reflectionDesc = m_d3dPipelineStateDesc;
	reflectionDesc.DepthStencilState = d;
	reflectionDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;

	hResult = pd3dDevice->CreateGraphicsPipelineState(&reflectionDesc,
		__uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CBillboardShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 1;

	m_ppGameObject = new CGameObject * [m_nGameObjects];

	m_ppGameObject[0] = new CBillboardObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppGameObject[0]->m_CollisionGroup = CG_IGNORE;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CBillboardShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i])
		{
			m_ppGameObject[i]->Render(pd3dCommandList, pCamera, m_nBillboard);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CSphereShader::CSphereShader()
{
	m_nSphere = 0;
}

CSphereShader::~CSphereShader()
{
}

D3D12_INPUT_LAYOUT_DESC CSphereShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;
	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CSphereShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "VSSphere", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CSphereShader::CreateGeometryShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "GSSphere", "gs_5_1", &m_pd3dGeometryShaderBlob));
}

D3D12_SHADER_BYTECODE CSphereShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "PSSphere", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CSphereShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.GS = CreateGeometryShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);


	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CSphereShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 1;
	m_ppGameObject = new CGameObject * [m_nGameObjects];

	m_ppGameObject[0] = new CSphereObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);;
	m_ppGameObject[0]->m_CollisionGroup = CG_IGNORE;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CSphereShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i])
		{
			m_ppGameObject[i]->Render(pd3dCommandList, pCamera, m_nSphere);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CBoundingBoxShader::CBoundingBoxShader()
{
	m_nBoxes = 0;
}

CBoundingBoxShader::~CBoundingBoxShader()
{
}

D3D12_INPUT_LAYOUT_DESC CBoundingBoxShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;
	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CBoundingBoxShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "VSBoundingBox", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CBoundingBoxShader::CreateGeometryShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "GSBoundingBox", "gs_5_1", &m_pd3dGeometryShaderBlob));
}

D3D12_SHADER_BYTECODE CBoundingBoxShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Debug.hlsl", "PSBoundingBox", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CBoundingBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.GS = CreateGeometryShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);


	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CBoundingBoxShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 1; 
	m_ppGameObject = new CGameObject * [m_nGameObjects]; 

	m_ppGameObject[0] = new CBoundingBoxObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);;
	m_ppGameObject[0]->m_CollisionGroup = CG_IGNORE;

	CreateShaderVariables(pd3dDevice, pd3dCommandList); 
}

void CBoundingBoxShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	for (int i = 0; i < m_nGameObjects; i++)
	{
		if (m_ppGameObject[i])
		{
			m_ppGameObject[i]->Render(pd3dCommandList, pCamera, m_nBoxes);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CUIShader::CUIShader()
{
	m_nUI = 0;
}

CUIShader::~CUIShader()
{
}

D3D12_DEPTH_STENCIL_DESC CUIShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC d = {};
	d.DepthEnable = FALSE;
	d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	d.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	d.StencilEnable = FALSE;
	d.StencilReadMask = 0x00;
	d.StencilWriteMask = 0x00;

	d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	d.BackFace = d.FrontFace;

	return d;
}

D3D12_BLEND_DESC CUIShader::CreateBlendState()
{
	D3D12_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;

	// 0번 RenderTarget 블렌딩 활성화
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].LogicOpEnable = FALSE;

	// 보통 UI는 srcAlpha / invSrcAlpha
	desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return desc;
}


D3D12_INPUT_LAYOUT_DESC CUIShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;
	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CUIShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"UI.hlsl", "VSUI", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CUIShader::CreateGeometryShader()
{
	return(CShader::CompileShaderFromFile(L"UI.hlsl", "GSUI", "gs_5_1", &m_pd3dGeometryShaderBlob));
}

D3D12_SHADER_BYTECODE CUIShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"UI.hlsl", "PSUI", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CUIShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.GS = CreateGeometryShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);


	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CUIShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 0;
	m_ppGameObject = nullptr;

	pUIDTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUIDTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/D_HUD.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUIDTexture, 0, 18);

	pUID2Texture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUID2Texture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/D2_HUD.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUID2Texture, 0, 19);

	pUIStartTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUIStartTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/Start.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUIStartTexture, 0, 20);

	pUIExitTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUIExitTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/Exit.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUIExitTexture, 0, 22);

	pUIHeightTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUIHeightTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/Height_HUD.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUIHeightTexture, 0, 23);

	pUIHeightNumTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pUIHeightNumTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image_UI/Num_HUD.dds", RESOURCE_TEXTURE2D, 0);
	CScene::CreateShaderResourceViews(pd3dDevice, pUIHeightNumTexture, 0, 24);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CUIShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	if (pUIDTexture)
		pUIDTexture->UpdateShaderVariables(pd3dCommandList);

	if (pUID2Texture)
		pUID2Texture->UpdateShaderVariables(pd3dCommandList);

	if (pUIStartTexture)
		pUIStartTexture->UpdateShaderVariables(pd3dCommandList);

	if (pUIExitTexture)
		pUIExitTexture->UpdateShaderVariables(pd3dCommandList);

	if (pUIHeightTexture)
		pUIHeightTexture->UpdateShaderVariables(pd3dCommandList);
	
	if (pUIHeightNumTexture)
		pUIHeightNumTexture->UpdateShaderVariables(pd3dCommandList);

	pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	pd3dCommandList->DrawInstanced(1, m_nUI, 0, 0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMirrorShader::CMirrorShader()
{
}

CMirrorShader::~CMirrorShader()
{
}

D3D12_DEPTH_STENCIL_DESC CMirrorShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC desc = {};

	desc.DepthEnable = TRUE;
	desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // 깊이 쓰기 금지
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	desc.StencilEnable = TRUE;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	desc.BackFace = desc.FrontFace;

	return desc;
}

D3D12_RASTERIZER_DESC CMirrorShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d = {};
	d.FillMode = D3D12_FILL_MODE_SOLID;
	d.CullMode = D3D12_CULL_MODE_NONE;
	d.DepthClipEnable = TRUE;
	return d;
}

D3D12_INPUT_LAYOUT_DESC CMirrorShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc = {};
	d3dInputLayoutDesc.pInputElementDescs = nullptr;
	d3dInputLayoutDesc.NumElements = 0;
	return d3dInputLayoutDesc;
}

D3D12_SHADER_BYTECODE CMirrorShader::CreateVertexShader()
{
	return(CShader::CompileShaderFromFile(L"Mirror.hlsl", "VSMirror", "vs_5_1", &m_pd3dVertexShaderBlob));
}

D3D12_SHADER_BYTECODE CMirrorShader::CreateGeometryShader()
{
	return(CShader::CompileShaderFromFile(L"Mirror.hlsl", "GSMirror", "gs_5_1", &m_pd3dGeometryShaderBlob));
}

D3D12_SHADER_BYTECODE CMirrorShader::CreatePixelShader()
{
	return(CShader::CompileShaderFromFile(L"Mirror.hlsl", "PSMirror", "ps_5_1", &m_pd3dPixelShaderBlob));
}

void CMirrorShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	::ZeroMemory(&m_d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	m_d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	m_d3dPipelineStateDesc.VS = CreateVertexShader();
	m_d3dPipelineStateDesc.GS = CreateGeometryShader();
	m_d3dPipelineStateDesc.PS = CreatePixelShader();
	m_d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	m_d3dPipelineStateDesc.BlendState = CreateBlendState();
	m_d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	m_d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	m_d3dPipelineStateDesc.SampleMask = UINT_MAX;
	m_d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	m_d3dPipelineStateDesc.NumRenderTargets = 1;
	m_d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	m_d3dPipelineStateDesc.SampleDesc.Count = 1;
	m_d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&m_d3dPipelineStateDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[0]);

	if (FAILED(hResult))
		OutputDebugString(L"[Reflection PSO] MIRROR FAILED!!!!\n");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC reflection = m_d3dPipelineStateDesc;

	reflection.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	reflection.DepthStencilState.DepthEnable = TRUE;
	reflection.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	reflection.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	reflection.DepthStencilState.StencilEnable = TRUE;
	reflection.DepthStencilState.StencilReadMask = 0xFF;
	reflection.DepthStencilState.StencilWriteMask = 0x00;

	reflection.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	reflection.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	reflection.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	reflection.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	reflection.DepthStencilState.BackFace = reflection.DepthStencilState.FrontFace;

	hResult = pd3dDevice->CreateGraphicsPipelineState(&reflection, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);

	if (FAILED(hResult))
		OutputDebugString(L"[Reflection PSO] MIRROR FAILED!!!!\n");

	if (m_pd3dVertexShaderBlob) m_pd3dVertexShaderBlob->Release();
	if (m_pd3dHullShaderBlob) m_pd3dHullShaderBlob->Release();
	if (m_pd3dDomainShaderBlob) m_pd3dDomainShaderBlob->Release();
	if (m_pd3dGeometryShaderBlob) m_pd3dGeometryShaderBlob->Release();
	if (m_pd3dPixelShaderBlob) m_pd3dPixelShaderBlob->Release();

	if (m_d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[] m_d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CMirrorShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, void* pContext)
{
	m_nGameObjects = 1;

	m_ppGameObject = new CGameObject * [m_nGameObjects];

	m_ppGameObject[0] = new CRectObject(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	m_ppGameObject[0]->m_CollisionGroup = CG_STATIC;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CMirrorShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, int nPipelineState)
{
	OnPrepareRender(pd3dCommandList, nPipelineState);

	if (m_ppGameObject[0])
	{
		m_ppGameObject[0]->Render(pd3dCommandList, pCamera, 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
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