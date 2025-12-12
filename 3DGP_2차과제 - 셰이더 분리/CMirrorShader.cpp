#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CMirrorShader.h"

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

void CMirrorShader::CreatePipelineState(
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

void CMirrorShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CreatePipelineState(pd3dDevice, pd3dGraphicsRootSignature);
	CShader::CreateReflectionPipelineState(pd3dDevice, pd3dGraphicsRootSignature);

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
