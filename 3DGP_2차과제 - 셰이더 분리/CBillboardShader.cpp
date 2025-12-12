#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CBillboardShader.h"

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

void CBillboardShader::CreatePipelineState(
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

void CBillboardShader::CreateReflectionPipelineState(
	ID3D12Device* pd3dDevice,
	ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC reflectionDesc = m_d3dPipelineStateDesc;

	D3D12_DEPTH_STENCIL_DESC d = {};
	d.DepthEnable = TRUE;
	d.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

	d.StencilEnable = TRUE;
	d.StencilReadMask = 0xFF;
	d.StencilWriteMask = 0x00;

	d.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	d.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	d.BackFace = d.FrontFace;

	reflectionDesc.DepthStencilState = d;
	reflectionDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	HRESULT hResult = pd3dDevice->CreateGraphicsPipelineState(&reflectionDesc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3dPipelineStates[1]);
}

void CBillboardShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 2;
	m_ppd3dPipelineStates = new ID3D12PipelineState * [m_nPipelineStates];

	CreatePipelineState(pd3dDevice, pd3dGraphicsRootSignature);
	CreateReflectionPipelineState(pd3dDevice, pd3dGraphicsRootSignature);

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