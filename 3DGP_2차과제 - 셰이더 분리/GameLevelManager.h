#pragma once

#include "Player.h"
#include "Scene.h"

class GameLevelManager
{
public:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature = NULL;

	CScene* levelArray[3];
	CPlayer player[3];

	int currentLevel;
	int nextLevel;
public:
	GameLevelManager(ID3D12Device* pd3dDevice);
	~GameLevelManager();

public:
	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* pd3dDevice);
	ID3D12RootSignature* GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }

	CScene* GetCurrentLevel() { return levelArray[currentLevel]; }
	CPlayer* GetCurrentPlayer() { return &player[currentLevel]; }

	void SetCurrentLevel(int level) { currentLevel = level; }
	void SetNextLevel(int level) { nextLevel = level; }

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ReleaseScene();
	void ChangeLevel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
		ID3D12CommandQueue* pd3dCommandQueue, ID3D12CommandAllocator* pd3dCommandAllocator);
};

