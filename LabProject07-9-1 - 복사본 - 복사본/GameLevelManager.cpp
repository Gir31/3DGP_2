#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Scene.h"
#include "GameLevelManager.h"

GameLevelManager::GameLevelManager()
{
	for (int i = 0; i < 3; i++)
	{
		player[i] = CPlayer();
		levelArray[i] = CScene();
	}

	currentLevel = 0;
	nextLevel = -1;
}

GameLevelManager::~GameLevelManager()
{
	for (int i = 0; i < 3; i++)
	{
		player[i].Release();
		levelArray[i].ReleaseObjects();
	}
}	

void GameLevelManager::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	levelArray[currentLevel].BuildObjects(pd3dDevice, pd3dCommandList);
}

void GameLevelManager::ChangeLevel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	levelArray[currentLevel].ReleaseUploadBuffers();
	levelArray[currentLevel].ReleaseObjects();

	currentLevel = nextLevel;

	BuildObjects(pd3dDevice, pd3dCommandList);
}