#pragma once

#include "Player.h"
#include "Scene.h"

class GameLevelManager
{
public:
	GameLevelManager();
	~GameLevelManager();

public:
	CScene levelArray[3];
	CPlayer player[3];
	//CCamera* m_pCamera = NULL;

	int currentLevel;
	int nextLevel;

public:
	CScene* GetCurrentLevel() { return &levelArray[currentLevel]; }
	CPlayer* GetCurrentPlayer() { return &player[currentLevel]; }

	void SetCurrentLevel(int level) { currentLevel = level; }
	void SetNextLevel(int level) { nextLevel = level; }

	void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void ChangeLevel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
};

