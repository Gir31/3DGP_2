#pragma once
#include "Scene.h"

class MenuLevel : public CScene
{
public:
	MenuLevel() {}
	~MenuLevel() {}
public:
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature,
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

public:
	bool pickingUI(const XMFLOAT3& xmf3Center, const XMFLOAT2& xmf2Size, const int& mouseX, const int& mouseY);
};


class MainLevel : public CScene
{
public:
	MainLevel() {}
	~MainLevel() {}
public:
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature = NULL);
	virtual void Render(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		CCamera* pCamera, ID3D12RootSignature* pd3dGraphicsRootSignature,
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
};

