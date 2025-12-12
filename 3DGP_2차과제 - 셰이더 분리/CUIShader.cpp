#include "stdafx.h"
#include "DDSTextureLoader12.h"
#include "Scene.h"
#include "CUIShader.h"

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

void CUIShader::CreatePipelineState(
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

void CUIShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
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
