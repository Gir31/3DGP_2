#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CDebugShader.h"

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

void CSphereShader::CreatePipelineState(
	ID3D12Device* pd3dDevice,
	ID3D12RootSignature* pd3dGraphicsRootSignature)
{
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
}

void CSphereShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CreatePipelineState(pd3dDevice, pd3dGraphicsRootSignature);

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

void CBoundingBoxShader::CreatePipelineState(
	ID3D12Device* pd3dDevice,
	ID3D12RootSignature* pd3dGraphicsRootSignature)
{
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
}

void CBoundingBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CreatePipelineState(pd3dDevice, pd3dGraphicsRootSignature);

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