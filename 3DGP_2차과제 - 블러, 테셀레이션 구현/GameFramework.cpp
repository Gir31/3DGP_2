//-----------------------------------------------------------------------------
// File: CGameFramework.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "GameFramework.h"

CGameFramework::CGameFramework()
{
	m_pdxgiFactory = NULL;
	m_pdxgiSwapChain = NULL;
	m_pd3dDevice = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; i++) m_ppd3dSwapChainBackBuffers[i] = NULL;
	m_nSwapChainBufferIndex = 0;

	m_pd3dCommandAllocator = NULL;
	m_pd3dCommandQueue = NULL;
	m_pd3dCommandList = NULL;

	m_pd3dRtvDescriptorHeap = NULL;
	m_pd3dDsvDescriptorHeap = NULL;

	gnRtvDescriptorIncrementSize = 0;
	gnDsvDescriptorIncrementSize = 0;

	m_hFenceEvent = NULL;
	m_pd3dFence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_nWndClientWidth = ::g_nScreenWidth;
	m_nWndClientHeight = ::g_nScreenHeight;

	m_pPlayer = NULL;

	m_pGLM = NULL;

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

CGameFramework::~CGameFramework()
{
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateRtvAndDsvDescriptorHeaps();
	CreateSwapChain();
	CreateDepthStencilView();

	CoInitialize(NULL);

	BuildObjects();

	CreateComputeShaderResources();

	return(true);
}

void CGameFramework::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

#ifdef _WITH_CREATE_SWAPCHAIN_FOR_HWND
	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc;
	::ZeroMemory(&dxgiSwapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue, m_hWnd, &dxgiSwapChainDesc, &dxgiSwapChainFullScreenDesc, NULL, (IDXGISwapChain1 **)&m_pdxgiSwapChain);
#else
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgiFactory->CreateSwapChain(m_pd3dCommandQueue, &dxgiSwapChainDesc, (IDXGISwapChain **)&m_pdxgiSwapChain);
#endif
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	hResult = m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	CreateRenderTargetViews();
#endif
}

void CGameFramework::CreateDirect3DDevice()
{
	HRESULT hResult;

	UINT nDXGIFactoryFlags = 0;
#if defined(_DEBUG)
	ID3D12Debug *pd3dDebugController = NULL;
	hResult = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&pd3dDebugController);
	if (pd3dDebugController)
	{
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, __uuidof(IDXGIFactory4), (void **)&m_pdxgiFactory);

	IDXGIAdapter1 *pd3dAdapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice))) break;
	}

	if (!pd3dAdapter)
	{
		m_pdxgiFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pd3dAdapter);
		hResult = D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), (void **)&m_pd3dDevice);
	}

	::gnCbvSrvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	::gnRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	::gnDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	hResult = m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;

	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void **)&m_pd3dFence);
	for (UINT i = 0; i < m_nSwapChainBuffers; i++) m_nFenceValues[i] = 0;

	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	if (pd3dAdapter) pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList()
{
	HRESULT hResult;

	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc;
	::ZeroMemory(&d3dCommandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc, _uuidof(ID3D12CommandQueue), (void **)&m_pd3dCommandQueue);

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void **)&m_pd3dCommandAllocator);

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3dCommandAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void **)&m_pd3dCommandList);
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers + 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dRtvDescriptorHeap);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dDsvDescriptorHeap);
}

void CGameFramework::CreateRenderTargetViews()
{
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < m_nSwapChainBuffers; i++)
	{
		m_pdxgiSwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void **)&m_ppd3dSwapChainBackBuffers[i]);
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dSwapChainBackBuffers[i], NULL, d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += gnRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView()
{
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue, __uuidof(ID3D12Resource), (void **)&m_pd3dDepthStencilBuffer);

	D3D12_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	::ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, d3dDsvCPUDescriptorHandle);
}

void CGameFramework::ChangeSwapChainState()
{
	WaitForGpuComplete();

	BOOL bFullScreenState = FALSE;
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, NULL);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, NULL);

	DXGI_MODE_DESC dxgiTargetParameters;
	dxgiTargetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiTargetParameters.Width = m_nWndClientWidth;
	dxgiTargetParameters.Height = m_nWndClientHeight;
	dxgiTargetParameters.RefreshRate.Numerator = 60;
	dxgiTargetParameters.RefreshRate.Denominator = 1;
	dxgiTargetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiTargetParameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&dxgiTargetParameters);

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth, m_nWndClientHeight, dxgiSwapChainDesc.BufferDesc.Format, dxgiSwapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pGLM->GetCurrentLevel()) 
		if(m_pGLM->GetCurrentLevel()->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam))
		{
			m_pGLM->SetNextLevel((m_pGLM->currentLevel + 1) % 3);
			m_pGLM->ChangeLevel(m_pd3dDevice, m_pd3dCommandList, m_pd3dCommandQueue, m_pd3dCommandAllocator);

			m_pPlayer = m_pGLM->GetCurrentLevel()->m_pPlayer;
			m_pCamera = m_pPlayer->GetCamera();

			WaitForGpuComplete();
		}
	switch (nMessageID)
	{
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			::SetCapture(hWnd);
			::GetCursorPos(&m_ptOldCursorPos);
			break;
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			::ReleaseCapture();
			break;
		case WM_MOUSEMOVE:
			break;
		default:
			break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_pGLM->GetCurrentLevel()) 
		m_pGLM->GetCurrentLevel()->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	switch (nMessageID)
	{
		case WM_KEYUP:
			switch (wParam)
			{
				case VK_ESCAPE:
					::PostQuitMessage(0);
					break;
				case VK_RETURN:
					break;
				case VK_F5:
					m_pGLM->GetCurrentLevel()->CM->DebugSphereRender =
						!m_pGLM->GetCurrentLevel()->CM->DebugSphereRender;
					break;
				case VK_F6:
					m_pGLM->GetCurrentLevel()->CM->DebugBoundingBoxRender =
						!m_pGLM->GetCurrentLevel()->CM->DebugBoundingBoxRender;
					break;
				case VK_F9:
					ChangeSwapChainState();
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		case WM_ACTIVATE:
		{
			if (LOWORD(wParam) == WA_INACTIVE)
				m_GameTimer.Stop();
			else
				m_GameTimer.Start();
			break;
		}
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MOUSEMOVE:
			OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
            break;
        case WM_KEYDOWN:
        case WM_KEYUP:
			OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
	}
	return(0);
}

void CGameFramework::OnDestroy()
{
    ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDsvDescriptorHeap) m_pd3dDsvDescriptorHeap->Release();

	for (int i = 0; i < m_nSwapChainBuffers; i++) if (m_ppd3dSwapChainBackBuffers[i]) m_ppd3dSwapChainBackBuffers[i]->Release();
	if (m_pd3dRtvDescriptorHeap) m_pd3dRtvDescriptorHeap->Release();

	if (m_pd3dCommandAllocator) m_pd3dCommandAllocator->Release();
	if (m_pd3dCommandQueue) m_pd3dCommandQueue->Release();
	if (m_pd3dCommandList) m_pd3dCommandList->Release();

	if (m_pd3dFence) m_pd3dFence->Release();

	m_pdxgiSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgiSwapChain) m_pdxgiSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pdxgiFactory) m_pdxgiFactory->Release();

#if defined(_DEBUG)
	IDXGIDebug1	*pdxgiDebug = NULL;
	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void **)&pdxgiDebug);
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
#endif
}

void CGameFramework::BuildObjects()
{
	float a = XMConvertToDegrees(atan(0.5));


	m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	m_pGLM = new GameLevelManager(m_pd3dDevice);
	if (m_pGLM->GetCurrentLevel()) 
		m_pGLM->BuildObjects(m_pd3dDevice, m_pd3dCommandList);

	m_pPlayer = m_pGLM->GetCurrentLevel()->m_pPlayer;
	m_pCamera = m_pPlayer->GetCamera();

	m_pd3dCommandList->Close();
	ID3D12CommandList *ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pGLM->GetCurrentLevel()) 
		m_pGLM->GetCurrentLevel()->ReleaseUploadBuffers();
	if (m_pPlayer) m_pPlayer->ReleaseUploadBuffers();

	m_GameTimer.Reset();
}

void CGameFramework::ReleaseObjects()
{
	if (m_pPlayer) m_pPlayer->Release();

	if (m_pGLM->GetCurrentLevel()) 
		m_pGLM->GetCurrentLevel()->ReleaseObjects();
	if (m_pGLM->GetCurrentLevel()) delete m_pGLM->GetCurrentLevel();
}

void CGameFramework::ProcessInput()
{
	static UCHAR pKeysBuffer[256];
	bool bProcessedByScene = false;
	if (GetKeyboardState(pKeysBuffer) && m_pGLM->GetCurrentLevel()) 
		bProcessedByScene = m_pGLM->GetCurrentLevel()->ProcessInput(pKeysBuffer);
	if (!bProcessedByScene)
	{
		DWORD dwDirection = 0;
		if (pKeysBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeysBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeysBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeysBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeysBuffer[VK_SHIFT] & 0xF0) dwDirection |= DIR_UP;
		if (pKeysBuffer[VK_CONTROL] & 0xF0) dwDirection |= DIR_DOWN;

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;
		if (GetCapture() == m_hWnd)
		{
			SetCursor(NULL);
			GetCursorPos(&ptCursorPos);
			cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeysBuffer[VK_RBUTTON] & 0xF0)
					m_pPlayer->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			if (dwDirection) m_pPlayer->Move(dwDirection, 5.0f, true);
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	float fTimeElapsed = m_GameTimer.GetTimeElapsed();

	if (m_pGLM->GetCurrentLevel()) 
		m_pGLM->GetCurrentLevel()->AnimateObjects(fTimeElapsed, m_pCamera);

	m_pPlayer->Animate(fTimeElapsed, NULL);
}

void CGameFramework::WaitForGpuComplete()
{
	const UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CGameFramework::MoveToNextFrame()
{
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();
	//m_nSwapChainBufferIndex = (m_nSwapChainBufferIndex + 1) % m_nSwapChainBuffers;

	UINT64 nFenceValue = ++m_nFenceValues[m_nSwapChainBufferIndex];
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence, nFenceValue);

	if (m_pd3dFence->GetCompletedValue() < nFenceValue)
	{
		hResult = m_pd3dFence->SetEventOnCompletion(nFenceValue, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

//#define _WITH_PLAYER_TOP
void CGameFramework::FrameAdvance()
{
	m_GameTimer.Tick(0.0f);
	ProcessInput();
	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator, NULL);

	// =========================================================
	// 1. 중간 텍스처(m_pTextureRender)에 장면 렌더링
	// =========================================================

	// 중간 텍스처를 RenderTarget으로 전환
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_pTextureRender;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &barrier);

	// 렌더 타겟 설정 (백버퍼 대신 중간 텍스처 사용)
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle = m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(m_d3dRtvRenderCPUHandle, pfClearColor, 0, NULL);
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3dCommandList->OMSetRenderTargets(1, &m_d3dRtvRenderCPUHandle, TRUE, &d3dDsvCPUDescriptorHandle);

	// 실제 렌더링
	if (m_pGLM->GetCurrentLevel())
		m_pGLM->GetCurrentLevel()->Render(m_pd3dDevice, m_pd3dCommandList, m_pCamera, m_pGLM->GetGraphicsRootSignature(), d3dDsvCPUDescriptorHandle);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	// =========================================================
	// 2. 플레이어 속도 체크 및 블러 처리
	// =========================================================

	XMFLOAT3 vel = m_pPlayer->GetVelocity();
	float fSpeed = sqrt(vel.x * vel.x + vel.y * vel.y + vel.z * vel.z);
	bool bApplyBlur = (fSpeed > 30.0f); // 속도 임계값 (조절 가능)

	// 백버퍼를 복사 받을 상태(Copy Dest)로 준비
	barrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	m_pd3dCommandList->ResourceBarrier(1, &barrier);

	if (bApplyBlur)
	{
		// [블러 적용]
		D3D12_RESOURCE_BARRIER barriers[2];
		// 1) RenderTex -> SRV (Input)
		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[0].Transition.pResource = m_pTextureRender;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		// 2) UAVTex -> UAV (Output)
		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[1].Transition.pResource = m_pTextureUAV;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		m_pd3dCommandList->ResourceBarrier(2, barriers);

		// Compute Shader 설정
		m_pd3dCommandList->SetComputeRootSignature(m_pComputeRootSignature);
		ID3D12DescriptorHeap* ppHeaps[] = { m_pCbvSrvUavDescriptorHeap };
		m_pd3dCommandList->SetDescriptorHeaps(1, ppHeaps);

		// 상수(강도), SRV, UAV 설정
		float blurStrength = (fSpeed - 30.0f) * 0.0005f; // 강도 계수
		m_pd3dCommandList->SetComputeRoot32BitConstants(0, 1, &blurStrength, 0);
		m_pd3dCommandList->SetComputeRootDescriptorTable(1, m_d3dSrvRenderGPUHandle);
		m_pd3dCommandList->SetComputeRootDescriptorTable(2, m_d3dUavGPUHandle);

		// 실행 (32x32 쓰레드 그룹)
		m_pBlurShader->Dispatch(m_pd3dCommandList, (m_nWndClientWidth + 31) / 32, (m_nWndClientHeight + 31) / 32, 1);

		// 결과(UAV)를 백버퍼로 복사하기 위해 상태 전환
		barrier.Transition.pResource = m_pTextureUAV;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		m_pd3dCommandList->ResourceBarrier(1, &barrier);

		m_pd3dCommandList->CopyResource(m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_pTextureUAV);

		// UAV 텍스처 상태 원복
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
		m_pd3dCommandList->ResourceBarrier(1, &barrier);
	}
	else
	{
		// [블러 없음] RenderTex -> 백버퍼 단순 복사
		barrier.Transition.pResource = m_pTextureRender;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
		m_pd3dCommandList->ResourceBarrier(1, &barrier);

		m_pd3dCommandList->CopyResource(m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex], m_pTextureRender);
	}

	// =========================================================
	// 3. 마무리 (Present 준비 및 리소스 정리)
	// =========================================================

	// 백버퍼: Copy Dest -> Present
	barrier.Transition.pResource = m_ppd3dSwapChainBackBuffers[m_nSwapChainBufferIndex];
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_pd3dCommandList->ResourceBarrier(1, &barrier);

	// 중간 텍스처: 사용 후 상태 원복 (다음 프레임을 위해 COMMON으로)
	barrier.Transition.pResource = m_pTextureRender;
	barrier.Transition.StateBefore = bApplyBlur ? D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_COPY_SOURCE;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COMMON;
	m_pd3dCommandList->ResourceBarrier(1, &barrier);

	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

#ifdef _WITH_PRESENT_PARAMETERS
	DXGI_PRESENT_PARAMETERS dxgiPresentParameters = { 0 };
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
#else
	m_pdxgiSwapChain->Present(0, 0);
#endif

	MoveToNextFrame();

	// FPS 표시 코드 등 기존 유지...
	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	size_t nLength = _tcslen(m_pszFrameRate);
	XMFLOAT3 xmf3Position = m_pPlayer->GetPosition();
	_stprintf_s(m_pszFrameRate + nLength, 70 - nLength, _T("(%4f, %4f, %4f)"), xmf3Position.x, xmf3Position.y, xmf3Position.z);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}


void CGameFramework::CreateComputeShaderResources()
{
	// 1. Compute Root Signature 생성 (Root Constant 1개 + Table(SRV, UAV))
	D3D12_DESCRIPTOR_RANGE ranges[2];
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; // t0
	ranges[0].NumDescriptors = 1;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV; // u0
	ranges[1].NumDescriptors = 1;
	ranges[1].BaseShaderRegister = 0;
	ranges[1].RegisterSpace = 0;
	ranges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER params[3];
	// Param 0: Root Constant (블러 강도)
	params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	params[0].Constants.ShaderRegister = 0; // b0
	params[0].Constants.RegisterSpace = 0;
	params[0].Constants.Num32BitValues = 1;
	params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Param 1: SRV (Input Texture)
	params[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[1].DescriptorTable.NumDescriptorRanges = 1;
	params[1].DescriptorTable.pDescriptorRanges = &ranges[0];
	params[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// Param 2: UAV (Output Texture)
	params[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	params[2].DescriptorTable.NumDescriptorRanges = 1;
	params[2].DescriptorTable.pDescriptorRanges = &ranges[1];
	params[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_DESC desc = { _countof(params), params, 0, NULL, D3D12_ROOT_SIGNATURE_FLAG_NONE };
	ID3DBlob* signature;
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, NULL);
	m_pd3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&m_pComputeRootSignature);
	signature->Release();

	// 2. Compute Shader 생성
	m_pBlurShader = new CComputeShader();
	m_pBlurShader->CreateShader(m_pd3dDevice, m_pComputeRootSignature, L"Blur.hlsl", "CSMain");

	// 3. 텍스처 리소스 생성 (m_pTextureRender, m_pTextureUAV)
	D3D12_RESOURCE_DESC texDesc = {};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Width = m_nWndClientWidth;
	texDesc.Height = m_nWndClientHeight;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	// Render용 (RTV + SRV)
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	D3D12_CLEAR_VALUE clearVal = { DXGI_FORMAT_R8G8B8A8_UNORM, { 0.0f, 0.125f, 0.3f, 1.0f } };

	D3D12_HEAP_PROPERTIES heapProp = { D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 1, 1 };
	m_pd3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, &clearVal, __uuidof(ID3D12Resource), (void**)&m_pTextureRender);

	// UAV용 (UAV + CopySrc)
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	m_pd3dDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COMMON, NULL, __uuidof(ID3D12Resource), (void**)&m_pTextureUAV);

	// 4. Descriptor Heap 생성 (CBV_SRV_UAV) - 2개 (SRV 1개 + UAV 1개)
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 };
	m_pd3dDevice->CreateDescriptorHeap(&heapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pCbvSrvUavDescriptorHeap);

	// 5. 뷰 생성 (RTV, SRV, UAV)
	// 5-1. RTV (m_pTextureRender용) - 기존 RTV 힙의 마지막 위치에 생성
	m_d3dRtvRenderCPUHandle = m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dRtvRenderCPUHandle.ptr += (m_nSwapChainBuffers * ::gnRtvDescriptorIncrementSize); // 백버퍼 개수만큼 건너뜀
	m_pd3dDevice->CreateRenderTargetView(m_pTextureRender, NULL, m_d3dRtvRenderCPUHandle);

	// 5-2. SRV (m_pTextureRender용) -> 힙의 0번 슬롯
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_pCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dSrvRenderGPUHandle = m_pCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	m_pd3dDevice->CreateShaderResourceView(m_pTextureRender, &srvDesc, cpuHandle);

	// 5-3. UAV (m_pTextureUAV용) -> 힙의 1번 슬롯
	cpuHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
	m_d3dUavGPUHandle = m_d3dSrvRenderGPUHandle;
	m_d3dUavGPUHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	m_pd3dDevice->CreateUnorderedAccessView(m_pTextureUAV, NULL, &uavDesc, cpuHandle);
}