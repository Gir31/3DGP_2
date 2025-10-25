#pragma once
#include "Scene.h"

class MainLevel : public CScene
{
public:
    MainLevel() {};
	~MainLevel() {};

public:
    // Input Handling (입력 처리)
    virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
    virtual bool ProcessInput(UCHAR* pKeysBuffer);

    // Rendering & Shader (렌더링 및 셰이더 관련)
    virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseShaderVariables();

    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

    // Object & Resource Management (오브젝트 및 리소스 관리)
    virtual void BuildDefaultLightsAndMaterials();
    virtual void BuildObject(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual void ReleaseObject();
    virtual void ReleaseUploadBuffers();

    // Animation & Update (애니메이션 및 갱신)
    virtual void AnimateObjects(float fTimeElapsed);
};

